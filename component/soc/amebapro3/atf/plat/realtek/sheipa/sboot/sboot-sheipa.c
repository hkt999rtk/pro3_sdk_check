#include <arch.h>
#include <arch_helpers.h>
#include <assert.h>
#include <common/debug.h>
#include <platform_def.h>
#include <stdint.h>
#include <string.h>

#include "ameba_vector.h"
#include "ameba_gdma.h"
#include "ameba_key_management_hmac.h"
#include "ameba_sha.h"

#include "ameba_crypto.h"
#include "ameba_pke.h"

#include "sboot-sheipa.h"

int sboot_validate_pubkey(u8 auth_alg, u8 *pubkey, u8 *pk_hash)
{
	int ret = -1;
	u32 pubkey_size, temp;
	u8 calc_hash[32];
	SHA_context sha_ctx;
	pke_ecp_group grp;

	if (IS_EDDSA(auth_alg)) {
		pubkey_size = 32;
	} else if (IS_ECDSA(auth_alg)) {
		pke_ecp_group_init_in_rom(&grp, auth_alg);
		if (auth_alg == AUTHID_ECDSA_SECP224K1) {
			// precise_bits of secp224k1 is 225, but public key is 224 bits
			temp = 224 / 8;
		} else {
			temp = (grp.precise_bits + 7) / 8;
		}
		pubkey_size = temp * 2;
	} else {
		return -1;
	}

	CRYPTO_CHK(crypto_sha2_init(&sha_ctx, SHA_256), -1);
	CRYPTO_CHK(crypto_sha2_update(&sha_ctx, pubkey, NULL, pubkey_size), -1);
	CRYPTO_CHK(crypto_sha2_final(&sha_ctx, calc_hash), -1);

	if (memcmp(pk_hash, calc_hash, 32) == 0) {
		return 0;
	} else {
		return -1;
	}

exit:
	return ret;
}

int sboot_get_md_type(u8 hash_id, u8 *md_type, u8 *is_hmac, u8 *hash_len)
{
	switch (hash_id) {
	case HASHID_SHA256:
		*md_type = SHA_256;
		*hash_len = SHA_DIGEST_256;
		*is_hmac = 0;
		break;
	case HASHID_SHA384:
		*md_type = SHA_384;
		*hash_len = SHA_DIGEST_384;
		*is_hmac = 0;
		break;
	case HASHID_SHA512:
		*md_type = SHA_512;
		*hash_len = SHA_DIGEST_512;
		*is_hmac = 0;
		break;
	case HASHID_HMAC256:
		*md_type = SHA_256;
		*hash_len = SHA_DIGEST_256;
		*is_hmac = 1;
		break;
	case HASHID_HMAC384:
		*md_type = SHA_384;
		*hash_len = SHA_DIGEST_384;
		*is_hmac = 1;
		break;
	case HASHID_HMAC512:
		*md_type = SHA_512;
		*hash_len = SHA_DIGEST_512;
		*is_hmac = 1;
		break;
	default:
		return -1;
	}

	return 0;
}

int sboot_validate_img_hash(u8 hash_alg, u8 *img_hash, IMAGE_HEADER *img_hdr)
{
	int ret = -1;
	u8 md_type, is_hmac = 0, hash_len = 0;
	u8 calc_hash[32];
	SHA_context sha_ctx;
	u32 key_id = KM_HMAC_KEY_S_IPSEC_KEY2;
	u32 key_len = SHA_HMAC_KEY_BIT_256;

	if (sboot_get_md_type(hash_alg, &md_type, &is_hmac, &hash_len) != 0) {
		return -1;
	}

	if (is_hmac) {
		CRYPTO_CHK(crypto_hmac_sha2_init(&sha_ctx, md_type, key_id, key_len), -1);
		CRYPTO_CHK(crypto_hmac_sha2_update(&sha_ctx, (u8 *)img_hdr, NULL, sizeof(IMAGE_HEADER)), -1);
		CRYPTO_CHK(crypto_hmac_sha2_update(&sha_ctx, (u8 *)img_hdr->image_addr, NULL, img_hdr->image_size), -1);
		CRYPTO_CHK(crypto_hmac_sha2_final(&sha_ctx, calc_hash), -1);
	} else {
		CRYPTO_CHK(crypto_sha2_init(&sha_ctx, md_type), -1);
		CRYPTO_CHK(crypto_sha2_update(&sha_ctx, (u8 *)img_hdr, NULL, sizeof(IMAGE_HEADER)), -1);
		CRYPTO_CHK(crypto_sha2_update(&sha_ctx, (u8 *)img_hdr->image_addr, NULL, img_hdr->image_size), -1);
		CRYPTO_CHK(crypto_sha2_final(&sha_ctx, calc_hash), -1);
	}

	if (memcmp(img_hash, calc_hash, hash_len) == 0) {
		return 0;
	} else {
		return -1;
	}

exit:
	return ret;
}

void sboot_swap_data(u8 *out_buf, u8 *in_buf, u8 len)
{
	for (u32 i = 0; i < len; i++) {
		out_buf[i] = in_buf[len - 1 - i];
	}
}

int sboot_eddsa_verify(u8 *pubkey, u8 *msg, u32 msg_len, u8 *sig)
{
	u8 pk_buf[32];
	u8 sign_r_buf[32];
	u8 sign_s_buf[32];

	sboot_swap_data(pk_buf, pubkey, sizeof(pk_buf));
	/* load signature: (r,s) */
	sboot_swap_data(sign_r_buf, sig, sizeof(sign_r_buf));
	sboot_swap_data(sign_s_buf, &sig[32], sizeof(sign_s_buf));

	return pke_eddsa_ed25519_read_signature(msg, msg_len, pk_buf, sign_r_buf, sign_s_buf);
}

int sboot_ecdsa_verify(u8 auth_alg, u8 hash_alg, u8 *pubkey, u8 *msg, u32 msg_len, u8 *sig)
{
	int ret = -1;
	SHA_context sha_ctx;
	u8 sha_type, is_hmac, hash_len;
	u32 key_id = KM_HMAC_KEY_S_IPSEC_KEY2;
	u32 key_len = SHA_HMAC_KEY_BIT_256;

	u8 calc_hash[HASH_MAX_LEN];
	u8 hash_buf[HASH_MAX_LEN];

	pke_ecp_group grp;
	size_t Csize;
	u8 r[SBOOT_ECP_MAX_BYTES];
	u8 s[SBOOT_ECP_MAX_BYTES];
	u8 Qx[SBOOT_ECP_MAX_BYTES];
	u8 Qy[SBOOT_ECP_MAX_BYTES];

	/* calculate manifest hash, utilize the same hash algorithm with image hash */
	if (sboot_get_md_type(hash_alg, &sha_type, &is_hmac, &hash_len) != 0) {
		return -1;
	}

	if (is_hmac) {
		/* SHA engine auto-load HMAC key */
		CRYPTO_CHK(crypto_hmac_sha2_init(&sha_ctx, sha_type, key_id, key_len), -1);
		CRYPTO_CHK(crypto_hmac_sha2_update(&sha_ctx, msg, NULL, msg_len), -1);
		CRYPTO_CHK(crypto_hmac_sha2_final(&sha_ctx, calc_hash), -1);
	} else {
		CRYPTO_CHK(crypto_sha2_init(&sha_ctx, sha_type), -1);
		CRYPTO_CHK(crypto_sha2_update(&sha_ctx, msg, NULL, msg_len), -1);
		CRYPTO_CHK(crypto_sha2_final(&sha_ctx, calc_hash), -1);
	}

	/* get curve info from AuthID */
	pke_ecp_group_init_in_rom(&grp, auth_alg);
	if (auth_alg == AUTHID_ECDSA_SECP224K1) {
		/* SECP224K1 public key is 224-bits, but signature is 225-bits */
		Csize = 224 / 8;
		/* load pubkey: Q */
		sboot_swap_data(Qx, pubkey, Csize);
		sboot_swap_data(Qy, pubkey + Csize, Csize);

		Csize += 1;
		/* load signature: (r,s) */
		sboot_swap_data(r, sig, Csize);
		sboot_swap_data(s, sig + Csize, Csize);
	} else {
		Csize = (grp.precise_bits + 7) / 8;
		/* load pubkey: Q */
		sboot_swap_data(Qx, pubkey, Csize);
		sboot_swap_data(Qy, pubkey + Csize, Csize);
		/* load signature: (r,s) */
		sboot_swap_data(r, sig, Csize);
		sboot_swap_data(s, sig + Csize, Csize);
	}

	/* verify signature */
	pke_ecdsa_lalu_hash_process(hash_buf, calc_hash, hash_len, grp.precise_bits);
	ret = pke_ecdsa_read_signature(&grp, Qx, Qy, hash_buf, hash_len, r, s);

exit:
	return ret;
}

int sboot_validate_signature(u8 auth_alg, u8 hash_alg, u8 *pubkey, u8 *msg, u32 len, u8 *sig)
{
	int ret = -1;

	if (IS_EDDSA(auth_alg)) {
		/* ed25519 HW implementatioin */
		ret = sboot_eddsa_verify(pubkey, msg, len, sig);
	} else if (IS_ECDSA(auth_alg)) {
		/* ecdsa HW implementation */
		ret = sboot_ecdsa_verify(auth_alg, hash_alg, pubkey, msg, len, sig);
	}

	return ret;
}

u8 sboot_is_en(void)
{
	u32 lgc_ena = OTP_SW_LOG_BASE->ROM_BOOT_CONFIG0;
	u32 phy_dis = KEY_STORAGE_BASE->KEY_ST_454_457;

	if (
		(!(phy_dis & KEY_STORAGE_BIT_SEC_BOOT_DIS))				// physical, not disable
		|| (lgc_ena & OTP_SW_LOG_BIT_LOGIC_SECURE_BOOT_EN)		// logical, enable
	) {
		return 1;
	} else {
		return 0;
	}
}
