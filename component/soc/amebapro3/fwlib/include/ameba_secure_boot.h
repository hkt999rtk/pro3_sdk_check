/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _AMEBA_SECURITY_BOOT_H_
#define _AMEBA_SECURITY_BOOT_H_

#include "bootutil_misc.h"

#define SBOOT_ERR_AUTH_NOT_SUPPORT					-1
#define SBOOT_ERR_PK_HASH_INVALID					-2
#define SBOOT_ERR_SIGN_INVALID						-3
#define SBOOT_ERR_ECDSA_CURVE_NOT_SUPPORT			-4
#define SBOOT_ERR_HASH_NOT_SUPPORT					-6
#define SBOOT_ERR_IMG_HASH_INVALID					-7
#define SBOOT_ERR_PK_HASH_ENGINE_ERROR				-8
#define SBOOT_ERR_EDDSA_HASH_ENGINE					-9
#define SBOOT_ERR_ECDSA_HASH_ENGINE					-10
#define SBOOT_ERR_IMG_HASH_ENGINE					-11
#define SBOOT_ERR_AUTH_ALG_NOT_MATCH				-12
#define SBOOT_ERR_HASH_ALG_NOT_MATCH				-13
#define SBOOT_ERR_GLOBAL_CFI_FAIL					-14
#define SBOOT_ERR_OTF_CHK_FAIL   					-15
#define SBOOT_ERR_LOAD_IMG_FAIL   					-16
#define SBOOT_ERR_AES_GCM_ENGINE					-0x100

#define IMG_SIZE_INVALID							-0xFFFE
#define IMG_INVALID									-0xFFFF

/*ImgID_TypeDef*/
#define IMGID_BOOT	0
#define IMGID_APP	1
#define IMGID_DSP	2
#define IMGID_BL1	3

/*AuthAlg_TypeDef*/
#define AUTHID_EDDSA_25519				0
#define AUTHID_ECDSA_SECP192R1			1 /* PKE_ECP_CURVE_SECP192R1 */
#define AUTHID_ECDSA_SECP224R1			2
#define AUTHID_ECDSA_SECP256R1			3
#define AUTHID_ECDSA_SECP384R1			4
#define AUTHID_ECDSA_BP256R1			6
#define AUTHID_ECDSA_BP384R1			7
#define AUTHID_ECDSA_BP512R1			8
#define AUTHID_ECDSA_CURVE25519			9
#define AUTHID_ECDSA_SECP192K1			10
#define AUTHID_ECDSA_SECP224K1			11
#define AUTHID_ECDSA_SECP256K1			12

/* PQC_AuthAlg_TypeDef: 0xf0, 0xf1, 0xf2 */
#define AUTHID_PQC_ML_DSA_44			240
#define AUTHID_PQC_ML_DSA_65			241
#define AUTHID_PQC_ML_DSA_87			242
#define PQC_ML_DSA_44_PUBKEY_LEN        1312
#define PQC_ML_DSA_65_PUBKEY_LEN        1952
#define PQC_ML_DSA_87_PUBKEY_LEN        2592
#define PQC_ML_DSA_44_SIG_LEN        2420
#define PQC_ML_DSA_65_SIG_LEN        3309
#define PQC_ML_DSA_87_SIG_LEN        4627

#define AUTHID_PQC_LMS_H15_W4			243
#define PQC_LMS_H15_W4_PUBKEY_LEN  	    60   /* HSS L=1 pubkey: 4(L) + 4(type) + 4(OTS_type) + 16(I) + 32(T1) */
#define PQC_LMS_H15_W4_SIG_LEN    		2672 /* HSS L=1 sig:    4(Nspk=0) + LMS_sig(2668) */

#define AUTHID_PQC_SLH_DSA_192s			244
#define PQC_SLH_DSA_192s_PUBKEY_LEN     48
#define PQC_SLH_DSA_192s_SIG_LEN        16224


#define SBOOT_ECP_MAX_BITS				512   /**< Maximum bit size of ecdsa groups */
#define SBOOT_ECP_MAX_BYTES				( ( SBOOT_ECP_MAX_BITS + 7 ) / 8 )
#define SBOOT_ECP_MAX_PT_LEN			( 2 * SBOOT_ECP_MAX_BYTES)
#define SBOOT_ECP_BIT2BYTE(x)			( ( (x) + 7 ) / 8 )

#define IS_PQC_ML_DSA_65(x)     (x == AUTHID_PQC_ML_DSA_65)
#define IS_PQC_LMS_H15_W4(x)    (x == AUTHID_PQC_LMS_H15_W4)
#define IS_PQC_SLH_DSA_192s(x)  (x == AUTHID_PQC_SLH_DSA_192s)
#define IS_EDDSA(x)				(x == AUTHID_EDDSA_25519)
#define IS_ECDSA(x)				(x == AUTHID_ECDSA_SECP192R1) || \
								(x == AUTHID_ECDSA_SECP224R1) || \
								(x == AUTHID_ECDSA_SECP256R1) || \
								(x == AUTHID_ECDSA_SECP384R1) || \
								(x == AUTHID_ECDSA_BP256R1) || \
								(x == AUTHID_ECDSA_BP384R1) || \
								(x == AUTHID_ECDSA_BP512R1) || \
								(x == AUTHID_ECDSA_CURVE25519) || \
								(x == AUTHID_ECDSA_SECP192K1) || \
								(x == AUTHID_ECDSA_SECP224K1) || \
								(x == AUTHID_ECDSA_SECP256K1)

/*HashAlg_TypeDef*/
#define HASHID_SHA256			0
#define HASHID_SHA384			1
#define HASHID_SHA512			2
#define HASHID_HMAC256			3
#define HASHID_HMAC384			4
#define HASHID_HMAC512			5

#define HASH_MAX_LEN		64
#define PKEY_MAX_LEN		144 //((((2 * MBEDTLS_ECP_MAX_BYTES -1) >> 4) + 1) << 4) //144 byte (16-byte aligned, format type field not included)
#define SIGN_MAX_LEN		144 //((((2 * MBEDTLS_ECP_MAX_BYTES -1) >> 4) + 1) << 4) //144 byte (16-byte aligned, raw data format, not ASN.1)

/* Extended manifest sizes for PQC support */
#define MLDSA_PKEY_MAX_LEN_EXT		2592 // Extended public key length for ml-dsa-87 algorithms (16-byte aligned)
#define MLDSA_SIGN_MAX_LEN_EXT		4640 // Extended signature length for ml-dsa-87 algorithms (16-byte aligned)

#define LMS_PKEY_MAX_LEN_EXT		64   // Extended public key length for lms algorithms (16-byte aligned)
#define LMS_SIGN_MAX_LEN_EXT		2720 // Extended signature length for lms algorithms (16-byte aligned)

#define SLHDSA_PKEY_MAX_LEN_EXT		64    // Extended public key length for sl-dsa algorithms (16-byte aligned)
#define SLHDSA_SIGN_MAX_LEN_EXT		16384 // Extended signature length for sl-dsa algorithms (16-byte aligned)

/* Fault injection protection constants */
#define SBOOT_INVALID_MD_TYPE					0xFF
#define SBOOT_INVALID_HASH_LEN					0x00
#define SBOOT_INVALID_IS_HMAC					0xFF

/* Valid HMAC constants for fault injection protection */
#define SBOOT_VALID_IS_HMAC_FALSE				0x00
#define SBOOT_VALID_IS_HMAC_TRUE				0x01

#define MANIFEST_SIZE_4K_ALIGN		0x1000
#define MANIFEST_SIZE_8K_ALIGN		0x2000
#define MANIFEST_SIZE_20K_ALIGN		0x5000

/* SBOOT PQC extend stack size (ml-dsa-65 verify need 58K, ml-dsa-87 92K) */
#define SBOOT_PQC_STACK_SIZE        (64 * 1024)

/* Manifest version flags (Manifest.Ver, offset 16). Selects the on-flash manifest size.
 * 1: 4KB  (default)
 * 2: 8KB  (PQC extension, 0x2000)
 * 3: reserved for 0x3000 (12KB)
 * 4: reserved for 0x4000 (16KB)
 * 5: 20KB (0x5000)
 * 6+: reserved */
#define MANIFEST_VERSION_4KB		1
#define MANIFEST_VERSION_8KB		2
/* MANIFEST_VERSION 3 reserved for 0x3000 (12KB) */
/* MANIFEST_VERSION 4 reserved for 0x4000 (16KB) */
#define MANIFEST_VERSION_20KB		5

/* public key hash algo fixed to sha2-256 */
#define SBOOT_PUBKEY_HASH_SIZE      32

/* Manifest RSIP Configuration (RSIPConfig)
 * NOTE: Structure-level definitions only. The amebapro3 boot path does NOT consume
 * RSIPConfig yet; the RSIP mode is taken from OTP (SYSCFG_OTP_RSIPMode()). These
 * macros are kept byte-for-byte aligned with RTL8720F for future per-image RSIP work.
 */
#define MANIFEST_BOOTLOADER_IMG1_ID                1
#define MANIFEST_AP_NP_IMG2_ID                     2
#define MANIFEST_SECURE_IMG3_ID                    3
#define MANIFEST_IMG_ID_IS_VALID(x)                ((x == MANIFEST_BOOTLOADER_IMG1_ID) || \
													(x == MANIFEST_AP_NP_IMG2_ID) || \
													(x == MANIFEST_SECURE_IMG3_ID))

#define MANIFEST_RSIP_GET_IMG_MODE(id, cfg)        ((u32)(((cfg >> 2 * (id)) & 0x00000003)))  /* IMG1 is bit[7:6] */

/* These 3 mode define keep same with manifest.json */
#define MANIFEST_RSIP_CTR_MODE                     0  /* rsip CTR MODE */
#define MANIFEST_RSIP_XTS_MODE                     1  /* rsip XTS MODE */
#define MANIFEST_RSIP_GCM_MODE                     2  /* rsip GCM MODE */
#define MANIFEST_RSIP_INVALID_MODE                 3  /* rsip INVALID MODE */

#define MANIFEST_RSIP_GCM_4_BYTE_TAG               0  /* 4 byte gcm tag length */
#define MANIFEST_RSIP_GCM_8_BYTE_TAG               1  /* 8 byte gcm tag length */
#define MANIFEST_RSIP_GCM_16_BYTE_TAG              2  /* 16 byte gcm tag length */
#define MANIFEST_RSIP_GCM_INVALID_TAG              3  /* invalid gcm tag length */
#define MANIFEST_RSIP_CFG_GET_GCM_TAG_LEN(x)       ((u32)(((x >> 0) & 0x00000003)))  /* RSIPConfig bit[1:0]: gcm tag length. 2b`00: 4 byte; 2b`01: 8 byte; 2b`10: 16 byte; default 2b`11: invalid */

typedef struct {
	u32 Pattern[2];
	u8 Rsvd1[7];
	u8 RSIPConfig;
	u8 Ver;
	u8 ImgID;
	u8 AuthAlg;
	u8 HashAlg;
	u16 MajorImgVer;
	u16 MinorImgVer;
	u32 ImgSize;
	u32 SecEpoch;
	u8 RsipIV[16];
	u8 ImgHash[HASH_MAX_LEN];
	u8 SBPubKey[PKEY_MAX_LEN];
	u8 Signature[SIGN_MAX_LEN];
} Manifest_TypeDef;

typedef struct {
	u8 PQC_AuthAlg;                /* PQC authentication algorithm */
	u8 Rsvd2[3];
	u16 PQC_KeySize;               /* PQC public key size */
	u16 PQC_SigSize;               /* PQC signature size */
	u8 Rsvd3[8];
} Manifest_PQC_TypeDef;

typedef struct {
	u8 PQC_AuthAlg;                /* PQC authentication algorithm */
	u8 Rsvd2[3];
	u16 PQC_KeySize;               /* PQC public key size */
	u16 PQC_SigSize;               /* PQC signature size */
	u8 Rsvd3[8];
	u8 PQCPubKey[MLDSA_PKEY_MAX_LEN_EXT];    /* PQC public key (up to PKEY_MAX_LEN_EXT bytes) */
	u8 PQCSignature[MLDSA_SIGN_MAX_LEN_EXT]; /* PQC signature (up to SIGN_MAX_LEN_EXT bytes) */
} Manifest_PQC_ML_DSA_TypeDef;

typedef struct {
	u8 PQC_AuthAlg;                /* PQC authentication algorithm */
	u8 Rsvd2[3];
	u16 PQC_KeySize;               /* PQC public key size */
	u16 PQC_SigSize;               /* PQC signature size */
	u8 Rsvd3[8];
	u8 PQCPubKey[LMS_PKEY_MAX_LEN_EXT];    /* PQC public key (up to PKEY_MAX_LEN_EXT bytes) */
	u8 PQCSignature[LMS_SIGN_MAX_LEN_EXT]; /* PQC signature (up to SIGN_MAX_LEN_EXT bytes) */
} Manifest_PQC_LMS_TypeDef;

typedef struct {
	u8 PQC_AuthAlg;                /* PQC authentication algorithm */
	u8 Rsvd2[3];
	u16 PQC_KeySize;               /* PQC public key size */
	u16 PQC_SigSize;               /* PQC signature size */
	u8 Rsvd3[8];
	u8 PQCPubKey[SLHDSA_PKEY_MAX_LEN_EXT];    /* PQC public key (up to PKEY_MAX_LEN_EXT bytes) */
	u8 PQCSignature[SLHDSA_SIGN_MAX_LEN_EXT]; /* PQC signature (up to SIGN_MAX_LEN_EXT bytes) */
} Manifest_PQC_SLH_DSA_TypeDef;

typedef struct {
	u32 Addr;
	u32 Len;
} SubImgInfo_TypeDef;

_LONG_CALL_ void BOOT_ROM_Copy(void *__restrict dst0, const void *__restrict src0, size_t len0);

fih_ret SBOOT_Validate_Algorithm(u8 *AuthAlg, u8 *HashAlg, u8 ManiAuth, u8 ManiHash);
fih_ret SBOOT_Validate_PubKey(u8 AuthAlg, u8 *Pk, u8 *Hash);
fih_ret SBOOT_Validate_HASH_Algorithm(u8 *HashAlg, u8 ManiHash);
fih_ret SBOOT_Validate_Signature(u8 AuthAlg, u8 HashAlg, u8 *Pk, u8 *Msg, u32 Len, u8 *Sig);
fih_ret SBOOT_Validate_ImgHash(u8 HashAlg, u8 *ImgHash, SubImgInfo_TypeDef *SubImgInfo, u8 Num, u8 *RsipIV);
fih_ret SBOOT_SignatureVerify(u8 *PubKeyHash, Manifest_TypeDef *Manifest);
fih_ret SBOOT_SignatureVerify_PQC(u8 *PubKeyHash, Manifest_TypeDef *Manifest, u32 ImgAddr, u32 PQC_stack_addr);
fih_ret BOOT_ROM_CheckSbootECC_Disable(uint8_t *PubKeyHash);
fih_ret BOOT_ROM_CheckSbootPQC_Disable(uint8_t *PubKeyHash);
fih_ret BOOT_ROM_SignatureCheck(Manifest_TypeDef *Manifest, volatile fih_ret *fih_ecc_verify_enabled);
fih_ret BOOT_ROM_SignatureCheck_PQC(Manifest_TypeDef *Manifest, u32 ImgAddr, volatile fih_ret *fih_pqc_verify_enabled);
_LONG_CALL_ u32 SBOOT_GetManifestSize(Manifest_TypeDef *Manifest);
void SBOOT_Verify_BootFromRAM(void);
__NO_RETURN void SBOOT_Validate_Fail_Stuck(u32 wdg_fresh);
#endif

