#ifndef SBOOT_SHEIPA_H
#define SBOOT_SHEIPA_H

typedef struct {
	__IO uint32_t ROM_BOOT_CONFIG0;                             /*!< Register,  Address offset:0x0000 */
	__IO uint32_t ROM_BOOT_CONFIG1;                             /*!< Register,  Address offset:0x0004 */
	__IO uint32_t SYS_SW_RESV_2;                                /*!< Register,  Address offset:0x0008 */
	__IO uint32_t SYS_SW_RESV_3;                                /*!< Register,  Address offset:0x000C */
	__IO uint32_t SYS_SW_RESV_4;                                /*!< Register,  Address offset:0x0010 */
	__IO uint32_t SYS_SW_RESV_5;                                /*!< Register,  Address offset:0x0014 */
	__IO uint32_t SYS_SW_RESV_6;                                /*!< Register,  Address offset:0x0018 */
	__IO uint32_t SYS_SW_RESV_7;                                /*!< Register,  Address offset:0x001C */
} OTP_SW_LOG_TypeDef;

#define OTP_SW_LOG_BASE ((OTP_SW_LOG_TypeDef *) (0x40230600))

#define OTP_SW_LOG_BIT_LOGIC_SECURE_BOOT_EN         ((u32)0x00000001 << 20)                  /*!< RW    0x0
                                                                                              * Secure boot enable 
                                                                                              *  
                                                                                              * 0: disable 
                                                                                              * 1: enable 
                                                                                              */

typedef struct {
	__IO uint32_t KEY_ST_SEC_BOOT_HASH0_3_0;                    /*!< Register,  Address offset:0x0000 */
	__IO uint32_t KEY_ST_SEC_BOOT_HASH0_7_4;                    /*!< Register,  Address offset:0x0004 */
	__IO uint32_t KEY_ST_SEC_BOOT_HASH0_B_8;                    /*!< Register,  Address offset:0x0008 */
	__IO uint32_t KEY_ST_SEC_BOOT_HASH0_F_C;                    /*!< Register,  Address offset:0x000C */
	__IO uint32_t KEY_ST_SEC_BOOT_HASH0_13_10;                  /*!< Register,  Address offset:0x0010 */
	__IO uint32_t KEY_ST_SEC_BOOT_HASH0_17_14;                  /*!< Register,  Address offset:0x0014 */
	__IO uint32_t KEY_ST_SEC_BOOT_HASH0_1B_18;                  /*!< Register,  Address offset:0x0018 */
	__IO uint32_t KEY_ST_SEC_BOOT_HASH0_1F_1C;                  /*!< Register,  Address offset:0x001C */
	__IO uint32_t KEY_ST_SEC_BOOT_HASH1_3_0;                    /*!< Register,  Address offset:0x0020 */
	__IO uint32_t KEY_ST_SEC_BOOT_HASH1_7_4;                    /*!< Register,  Address offset:0x0024 */
	__IO uint32_t KEY_ST_SEC_BOOT_HASH1_B_8;                    /*!< Register,  Address offset:0x0028 */
	__IO uint32_t KEY_ST_SEC_BOOT_HASH1_F_C;                    /*!< Register,  Address offset:0x002C */
	__IO uint32_t KEY_ST_SEC_BOOT_HASH1_13_10;                  /*!< Register,  Address offset:0x0030 */
	__IO uint32_t KEY_ST_SEC_BOOT_HASH1_17_14;                  /*!< Register,  Address offset:0x0034 */
	__IO uint32_t KEY_ST_SEC_BOOT_HASH1_1B_18;                  /*!< Register,  Address offset:0x0038 */
	__IO uint32_t KEY_ST_SEC_BOOT_HASH1_1F_1C;                  /*!< Register,  Address offset:0x003C */
	__IO uint32_t KEY_ST_HUK_3_0;                               /*!< Register,  Address offset:0x0040 */
	__IO uint32_t KEY_ST_HUK_7_4;                               /*!< Register,  Address offset:0x0044 */
	__IO uint32_t KEY_ST_HUK_B_8;                               /*!< Register,  Address offset:0x0048 */
	__IO uint32_t KEY_ST_HUK_F_C;                               /*!< Register,  Address offset:0x004C */
	__IO uint32_t KEY_ST_454_457;                               /*!< Register,  Address offset:0x0050 */
	__IO uint32_t KEY_ST_458_45B;                               /*!< Register,  Address offset:0x0054 */
	__IO uint32_t KEY_ST_45C_45F;                               /*!< Register,  Address offset:0x0058 */
	__IO uint32_t KEY_ST_BOOTLOAD_VER_3_0;                      /*!< Register,  Address offset:0x005C */
	__IO uint32_t KEY_ST_BOOTLOAD_VER_7_4;                      /*!< Register,  Address offset:0x0060 */
	__IO uint32_t KEY_ST_BOOTLOAD_VER_B_8;                      /*!< Register,  Address offset:0x0064 */
	__IO uint32_t KEY_ST_BOOTLOAD_VER_F_C;                      /*!< Register,  Address offset:0x0068 */
	__IO uint32_t KEY_ST_RMA_STATE;                             /*!< Register,  Address offset:0x006C */
	__IO uint32_t RMA_BOOT_HASH_3_0;                            /*!< Register,  Address offset:0x0070 */
	__IO uint32_t RMA_BOOT_HASH_7_4;                            /*!< Register,  Address offset:0x0074 */
	__IO uint32_t RMA_BOOT_HASH_B_8;                            /*!< Register,  Address offset:0x0078 */
	__IO uint32_t RMA_BOOT_HASH_F_C;                            /*!< Register,  Address offset:0x007C */
	__IO uint32_t RMA_BOOT_HASH_13_10;                          /*!< Register,  Address offset:0x0080 */
	__IO uint32_t RMA_BOOT_HASH_17_14;                          /*!< Register,  Address offset:0x0084 */
	__IO uint32_t RMA_BOOT_HASH_1B_18;                          /*!< Register,  Address offset:0x0088 */
	__IO uint32_t RMA_BOOT_HASH_1F_1C;                          /*!< Register,  Address offset:0x008C */
	__IO uint32_t KEY_ST_RSVD_4D7_4D4;                          /*!< Register,  Address offset:0x0090 */
	__IO uint32_t KEY_ST_RSVD_4DB_4D8;                          /*!< Register,  Address offset:0x0094 */
	__IO uint32_t KEY_ST_RSVD_4DF_4DC;                          /*!< Register,  Address offset:0x0098 */
} KEY_STORAGE_TypeDef;

#define KEY_STORAGE_BASE ((KEY_STORAGE_TypeDef *) (0x4023E000))

#define KEY_STORAGE_BIT_SEC_BOOT_DIS                ((u32)0x00000001 << 14)                  /*!< R/W Once 0x0 Secure boot disable */

#define HASH_MAX_LEN		64
#define PKEY_MAX_LEN		144
#define SIGN_MAX_LEN		144

#define SBOOT_ECP_MAX_BITS				512   /**< Maximum bit size of ecdsa groups */
#define SBOOT_ECP_MAX_BYTES				( ( SBOOT_ECP_MAX_BITS + 7 ) / 8 )

#define CERT_MAX_KEYINFO	5

typedef struct {
	u32 signature[2];
	u32 image_size;
	u32 image_addr;

	u32 boot_index;
	u32 reserved[3];
} IMAGE_HEADER;

typedef struct {
	u32 pattern[2];
	u8 rsvd[7];
	u8 rsip_config;
	u8 ver;
	u8 img_id;
	u8 auth_alg;
	u8 hash_alg;
	u16 major_img_ver;
	u16 minor_img_ver;
	u32 img_size;
	u32 sec_epoch;
	u8 rsip_iv[16];
	u8 img_hash[HASH_MAX_LEN];
	u8 pubkey[PKEY_MAX_LEN];
	u8 signature[SIGN_MAX_LEN];
} MANIFEST;

typedef struct {
	u32 key_id;
	unsigned char hash[32];
} CERT_ENTRY;

typedef struct {
	u8 pattern[8];
	u8 rsvd[8];
	u8 ver;
	u8 img_id;
	u8 auth_alg;
	u8 hash_alg;
	u16 major_key_ver;
	u16 minor_key_ver;
	u32 table_size;
	u32 entry_num;
	u8 pubkey[PKEY_MAX_LEN];
	CERT_ENTRY pk_info[CERT_MAX_KEYINFO];
} CERTIFICATE;

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

int sboot_validate_pubkey(u8 auth_alg, u8 *pubkey, u8 *pk_hash);
int sboot_validate_signature(u8 auth_alg, u8 hash_alg, u8 *pubkey, u8 *msg, u32 len, u8 *sig);
int sboot_validate_img_hash(u8 hash_alg, u8 *img_hash, IMAGE_HEADER *img_hdr);
u8 sboot_is_en(void);

#endif
