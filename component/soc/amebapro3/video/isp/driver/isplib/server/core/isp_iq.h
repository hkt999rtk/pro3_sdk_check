#ifndef _ISP_IQ_H_INC_
#define _ISP_IQ_H_INC_

#include <rts_isp.h>
#include <rts_isp_dynamic.h>
#include <isp_iq_table.pb.h>
#include <isp_algo_param.pb.h>
#include <isp_utils.h>

#define _MAX_ISP_IQ_NUM 4
#define _MAX_ISP_IQ_BINARY_NUM 8

#define ISP_DYNAMIC_PRIORITY 0
#define ISP_DYNAMIC_PATCH_PRIORITY 1
#define ISP_DYNAMIC_POST_PRIORITY 2

extern isp_iq_table_t default_iq;

struct isp_iq_entry {
	char name[RTS_ISP_NAME_LEN];
	uint8_t day_id;
	uint8_t night_id;
};

struct isp_iq_binary_desc {
	uint32_t offset;
	uint32_t length;
};

struct isp_iq {
	int id;
	int dn;

	int mode;
	int scene;
	bool scene_has_set;
	bool outdoor;
	bool high_temp;

	struct isp_iq_entry *entries;
	struct isp_iq_binary_desc *binaries;
	isp_iq_table_t *iq_table;
	isp_algo_param_t algo_param;
	uint32_t packed_version;
	uint8_t cus_version;
	uint64_t timestamp;

	uint32_t entry_bits;

	uint32_t owner_id;
};

struct isp_iq_ctrl_info {
	uint32_t id;
	uint32_t entry_bits;
	const char *names[_MAX_ISP_IQ_NUM];
};

int isp_iq_init(struct isp_iq *iq, uint32_t isp_id);
int isp_iq_cleanup(struct isp_iq *iq);
int isp_iq_load(struct isp_iq *iq);
int isp_iq_reload(struct isp_iq *iq, int (*apply_iq_callback)(void *priv),
				  void *priv);
int isp_iq_load_from_shmem(struct isp_iq *iq);
int isp_iq_change_table(struct isp_iq *iq, int id);
int isp_iq_change_day_night(struct isp_iq *iq, int dn);
int isp_iq_set_mode(struct isp_iq *iq, int mode);
int isp_iq_set_scene(struct isp_iq *iq, int scene);
int isp_iq_set_outdoor(struct isp_iq *iq, int outdoor);
int isp_iq_set_high_temp(struct isp_iq *iq, int high_temp);
int isp_iq_get_ctrl_info(struct isp_iq *iq, struct isp_iq_ctrl_info *info);

isp_algo_param_t *isp_iq_get_algo_param(uint32_t isp_id);
isp_algo_param_header_t *isp_iq_get_algo_param_header(uint32_t isp_id);

int isp_iq_is_night(uint32_t isp_id);
int isp_iq_current_table(uint32_t isp_id);
const isp_iq_header_t *isp_iq_get_header(uint32_t isp_id);

#define DEFINE_ISP_IQ_INTP_FUNC(mod, ver, intp_cb)                             \
	static const isp_iq_##mod##_##ver##_dyn_item_t                         \
		*isp_iq_##mod##_##ver##_intp_cb(                               \
			int dyn_vals[3], int thds[6],                          \
			const isp_iq_##mod##_##ver##_dyn_item_t *items[4],     \
			int changed[3], void *priv)                            \
	{                                                                      \
		const isp_iq_##mod##_##ver##_dyn_item_t *pitem[2];             \
		struct isp_mod_##mod *mod = priv;                              \
                                                                               \
		switch (ARRAY_SIZE(mod->dyn_items)) {                          \
		case 0:                                                        \
			if (items[1] || items[2])                              \
				return NULL;                                   \
			break;                                                 \
		case 1:                                                        \
		case 2:                                                        \
			if (items[1] && items[2])                              \
				return NULL;                                   \
			break;                                                 \
		default:                                                       \
			break;                                                 \
		}                                                              \
                                                                               \
		if (items[0] && items[1]) {                                    \
			pitem[0] = &mod->dyn_items[1];                         \
			if (changed[0] || changed[1])                          \
				intp_cb(&mod->dyn_items[1], dyn_vals[1],       \
					thds[2], items[0], thds[3], items[1]); \
		} else {                                                       \
			pitem[0] = items[0];                                   \
		}                                                              \
		if (items[2] && items[3]) {                                    \
			pitem[1] = &mod->dyn_items[2];                         \
			if (changed[0] || changed[2])                          \
				intp_cb(&mod->dyn_items[2], dyn_vals[2],       \
					thds[4], items[2], thds[5], items[3]); \
		} else {                                                       \
			pitem[1] = items[2];                                   \
		}                                                              \
		if (pitem[1] && (changed[0] || changed[1] || changed[2])) {    \
			intp_cb(&mod->dyn_items[0], dyn_vals[0], thds[0],      \
				pitem[0], thds[1], pitem[1]);                  \
			return &mod->dyn_items[0];                             \
		} else {                                                       \
			return pitem[0];                                       \
		}                                                              \
	}

#define DECLARE_IQ_GET_DYN_ITEM_DEFAULT_FUNC(mod, ver)                     \
	const isp_iq_##mod##_##ver##_dyn_item_t                            \
		*isp_iq_get_##mod##_##ver##_dyn_item_default(              \
			const isp_iq_##mod##_##ver##_dyn_nest_t *dyn_nest, \
			isp_algo_param_common_t *common);

#define DECLARE_IQ_GET_FUNC(mod, ver)                             \
	const isp_iq_##mod##_##ver##_t *isp_iq_get_##mod##_##ver( \
		uint32_t isp_id, isp_algo_param_common_t *common);

#define DECLARE_IQ_GET_DYN_ITEM_FUNC_EX(mod, ver)                             \
	const isp_iq_##mod##_##ver##_dyn_item_t                               \
		*isp_iq_get_##mod##_##ver##_dyn_item_ex(                      \
			const isp_iq_##mod##_##ver##_dyn_nest_t *dyn_nest,    \
			isp_algo_param_common_t *common,                      \
			const struct isp_notify_dynamic *dynamic,             \
			const isp_iq_##mod##_##ver##_dyn_item_t *(            \
				*intp_iq_item_cb)(                            \
				int dyn_vals[3], int thds[6],                 \
				const isp_iq_##mod##_##ver##_dyn_item_t       \
					*items[4],                            \
				int changed[3], void *intp_priv),             \
			void *intp_priv,                                      \
			int (*bounds_active_cb)(int type, void *bounds_priv), \
			void *bounds_priv);                                   \
	int isp_iq_##mod##_##ver##_dyn_enabled(                               \
		const isp_iq_##mod##_##ver##_dyn_nest_t *nest);

#define DECLARE_IQ_GET_DYN_ITEM_FUNC(mod, ver)                             \
	const isp_iq_##mod##_##ver##_dyn_item_t                            \
		*isp_iq_get_##mod##_##ver##_dyn_item(                      \
			const isp_iq_##mod##_##ver##_dyn_nest_t *dyn_nest, \
			isp_algo_param_common_t *common,                   \
			const struct isp_notify_dynamic *dynamic,          \
			const isp_iq_##mod##_##ver##_dyn_item_t *(         \
				*intp_iq_item_cb)(                         \
				int dyn_vals[3], int thds[6],              \
				const isp_iq_##mod##_##ver##_dyn_item_t    \
					*items[4],                         \
				int changed[3], void *priv),               \
			void *priv);                                       \
	int isp_iq_##mod##_##ver##_dyn_enabled(                            \
		const isp_iq_##mod##_##ver##_dyn_nest_t *nest);

#define DECLARE_IQ_GET_ALGO_PARAM_FUNC(mod, ver)                            \
	isp_algo_param_##mod##_##ver##_t *isp_algo_param_get_##mod##_##ver( \
		uint32_t isp_id);

#define DECLARE_IQ_NON_DYN_COMMON_FUNCS(mod, ver) \
	DECLARE_IQ_GET_ALGO_PARAM_FUNC(mod, ver)  \
	DECLARE_IQ_GET_FUNC(mod, ver)

#define DECLARE_IQ_COMMON_FUNCS(mod, ver)         \
	DECLARE_IQ_NON_DYN_COMMON_FUNCS(mod, ver) \
	DECLARE_IQ_GET_DYN_ITEM_FUNC(mod, ver)    \
	DECLARE_IQ_GET_DYN_ITEM_DEFAULT_FUNC(mod, ver)

DECLARE_IQ_COMMON_FUNCS(awb, v1)
DECLARE_IQ_COMMON_FUNCS(blc, v1)
DECLARE_IQ_COMMON_FUNCS(ccm, v1)
DECLARE_IQ_COMMON_FUNCS(gamma, v1)
DECLARE_IQ_COMMON_FUNCS(ygc, v1)
DECLARE_IQ_COMMON_FUNCS(uvtune, v1)
DECLARE_IQ_COMMON_FUNCS(spe, v1)
DECLARE_IQ_COMMON_FUNCS(ygamma, v1)
DECLARE_IQ_COMMON_FUNCS(eeh, v1)
DECLARE_IQ_COMMON_FUNCS(daynight, v1)

DECLARE_IQ_NON_DYN_COMMON_FUNCS(nlsc, v1)
DECLARE_IQ_GET_DYN_ITEM_FUNC_EX(nlsc, v1)
DECLARE_IQ_GET_DYN_ITEM_DEFAULT_FUNC(nlsc, v1)

DECLARE_IQ_NON_DYN_COMMON_FUNCS(mlsc, v1)
DECLARE_IQ_NON_DYN_COMMON_FUNCS(ae, v1)
DECLARE_IQ_NON_DYN_COMMON_FUNCS(af, v1)
DECLARE_IQ_NON_DYN_COMMON_FUNCS(wdr, v1)
DECLARE_IQ_NON_DYN_COMMON_FUNCS(high_temp, v1)
DECLARE_IQ_NON_DYN_COMMON_FUNCS(flick, v1)

DECLARE_IQ_NON_DYN_COMMON_FUNCS(texture, v1)
DECLARE_IQ_GET_DYN_ITEM_FUNC(texture, v1)

#endif /* _ISP_IQ_H_INC_ */
