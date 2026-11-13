/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2018 Grant Shen <grant_shen@realsil.com.cn>
 */

#include <pb_decode.h>
#include <isp_os_log.h>
#include <isp_os_mem.h>
#include <isp_utils.h>
#include "isp_top.h"
#include "isp_core.h"
#include "isp_plugin.h"
#include "isp_iq.h"

#define ISP_IQ_TABLE_SIZE_MAX (128 * 1024)

#define ISP_PACKED_IQ_MAGIC_NUM 0x504B4951 /* 'P', 'K', 'I', 'Q' */
#define ISP_PACKED_IQ_VERSION 0x00000001

struct isp_packed_iq_header {
	uint32_t magic;
	uint32_t version;
	uint64_t timestamp;
	uint8_t group_num;
	uint8_t def_group;
	uint8_t binary_num;
	uint8_t cus_version;
	uint8_t reserved[12];
} __attribute__((packed));

struct isp_packed_iq_group {
	uint8_t name[RTS_ISP_NAME_LEN];
	uint8_t day_id;
	uint8_t night_id;
	uint8_t reserved[14];
} __attribute__((packed));

struct isp_packed_iq_binary {
	uint32_t offset;
	uint32_t length;
	uint8_t extend_length;
	uint8_t reserved[7];
} __attribute__((packed));

static void iq_reset_info(struct isp_iq *iq)
{
	iq->id = 0;
	iq->entry_bits = 0;
	isp_free(iq->entries);
	isp_free(iq->binaries);
}

static void iq_log_info(struct isp_iq *iq)
{
	union iq_tm {
		uint64_t timestamp;
		struct {
			uint16_t year;
			uint8_t month;
			uint8_t day;
			uint8_t hour;
			uint8_t minute;
			uint8_t second;
		};
	} time;

	time.timestamp = iq->timestamp;

	isp_info(
		"load packed iq, version: 0x%08x, cus_version: 0x%02x, timestamp: %04d-%02d-%02d %02d:%02d:%02d\n",
		iq->packed_version, iq->cus_version, time.year, time.month,
		time.day, time.hour, time.minute, time.second);
}

int isp_iq_init(struct isp_iq *iq, uint32_t isp_id)
{
	if (!iq || isp_id >= _ISP_ID_NUM) {
		return -RTS_ISP_EINVAL;
	}
	isp_memset(iq, 0, sizeof(*iq));
	iq->algo_param.header.magic = ISP_ALGO_PARAM_HEADER_MAGIC;
	iq->algo_param.header.version = ISP_ALGO_PARAM_HEADER_VERSION;
	iq->owner_id = isp_id;

	return RTS_ISP_OK;
}

int isp_iq_cleanup(struct isp_iq *iq)
{
	if (!iq) {
		return -RTS_ISP_EINVAL;
	}
	iq_reset_info(iq);
	pb_release(isp_iq_table_t_fields, iq->iq_table);
	isp_free(iq->iq_table);
	isp_memset(iq, 0, sizeof(*iq));
	return RTS_ISP_OK;
}

int isp_iq_load(struct isp_iq *iq)
{
	int i;
	int ret;
	const void *handle;
	const struct isp_packed_iq_header *header;
	const struct isp_packed_iq_group *group;
	const struct isp_packed_iq_binary *binary;

	if (!iq) {
		return -RTS_ISP_EINVAL;
	}

	iq_reset_info(iq);

	handle = isp_plugin_handle(iq->owner_id, RTS_ISP_PLUG_TYPE_IQ);
	if (!handle) {
		return -RTS_ISP_ENOTREADY;
	}

	header = handle;
	if (header->magic != ISP_PACKED_IQ_MAGIC_NUM ||
		header->version != ISP_PACKED_IQ_VERSION ||
		header->group_num > _MAX_ISP_IQ_NUM ||
		header->binary_num > _MAX_ISP_IQ_BINARY_NUM ||
		header->def_group >= header->group_num) {
		ret = -RTS_ISP_EINVAL;
		goto out;
	}

	iq->id = header->def_group;
	iq->packed_version = header->version;
	iq->cus_version = header->cus_version;
	iq->timestamp = header->timestamp;
	isp_calloc_array(iq->entries, header->group_num);
	isp_calloc_array(iq->binaries, header->binary_num);
	if (!iq->entries || !iq->binaries) {
		ret = -RTS_ISP_ENOMEM;
		goto out;
	}

	group = (void *)header + sizeof(*header);
	for (i = 0; i < header->group_num; i++) {
		if (iq->entry_bits & (1 << i) ||
			group[i].day_id >= header->binary_num ||
			group[i].night_id >= header->binary_num) {
			ret = -RTS_ISP_EINVAL;
			goto out;
		}
		iq->entry_bits |= 1 << i;
		isp_memcpy(iq->entries[i].name, group[i].name,
				   RTS_ISP_NAME_LEN);
		iq->entries[i].day_id = group[i].day_id;
		iq->entries[i].night_id = group[i].night_id;
	}
	binary = (void *)group + sizeof(*group) * header->group_num;
	for (i = 0; i < header->binary_num; i++) {
		iq->binaries[i].offset = binary[i].offset;
		iq->binaries[i].length = binary[i].length;
	}
	ret = isp_iq_load_from_shmem(iq);
out:
	if (ret) {
		iq_reset_info(iq);
		isp_perror(ret, "load packed iq fail");
	} else {
		iq_log_info(iq);
	}
	return ret;
}

static void isp_iq_release(struct isp_iq *iq)
{
	if (!iq || !iq->iq_table) {
		return;
	}
	pb_release(isp_iq_table_t_fields, iq->iq_table);
	isp_free(iq->entries);
	isp_free(iq->binaries);
	isp_free(iq->iq_table);
}

int isp_iq_reload(struct isp_iq *iq, int (*apply_iq_callback)(void *priv),
				  void *priv)
{
	int ret;

	isp_iq_release(iq);
	ret = isp_iq_load(iq);
	if (ret) {
		goto out;
	}
	ret = apply_iq_callback(priv);
out:
	if (ret) {
		isp_iq_release(iq);
		apply_iq_callback(priv);
	}
	return ret;
}
#define USE_PB_DECODE 1
#if !USE_PB_DECODE
#define BLOCK_NUM 64 //Default
#define MPOOL_HEAD_DATA_NUM 3
struct alloc_addr_info {
	unsigned int ptr_mallocer;
	unsigned int mapto;
};

struct pool_info {
	unsigned int request_size;
	unsigned int malloc_size;
	int ptr_blk_index;
	int ptr_blk_res;
	struct alloc_addr_info addr_info_old;
	struct alloc_addr_info addr_info_new;
};
void pool_update_ptr(isp_iq_table_t *iqtable, struct pool_info *poolinfo)
{
	int i;
	for (i = 0; i < BLOCK_NUM; i++) {
		int abc;
		unsigned int ptr_offset_new = (unsigned int)iqtable + poolinfo[i].addr_info_new.ptr_mallocer;
		unsigned int block_start = (unsigned int)iqtable + poolinfo[i].addr_info_new.mapto;
		*(unsigned int *)ptr_offset_new = (block_start + sizeof(size_t) * MPOOL_HEAD_DATA_NUM);

		*(unsigned int *)(block_start + 0) = poolinfo[i].request_size + sizeof(size_t) * MPOOL_HEAD_DATA_NUM;
		*(unsigned int *)(block_start + 4) = ptr_offset_new;
		*(unsigned int *)(block_start + 8) = poolinfo[i].request_size;
		abc = 0;
	}
}
#endif
int isp_iq_load_from_shmem(struct isp_iq *iq)
{
	int ret;
	uint32_t offset;
	uint32_t length;
	const uint8_t *buffer;
	pb_istream_t stream;
	isp_iq_table_t *iq_table;
	isp_iq_table_t *tmp;

	if (!isp_calloc_item(iq_table)) {
		return -RTS_ISP_ENOMEM;
	}
	if (!isp_test_bit(iq->entry_bits, iq->id)) {
		ret = -RTS_ISP_ENOTREADY;
		goto out;
	}
	if (iq->dn == RTS_ISP_DAY) {
		offset = iq->binaries[iq->entries[iq->id].day_id].offset;
		length = iq->binaries[iq->entries[iq->id].day_id].length;
	} else {
		offset = iq->binaries[iq->entries[iq->id].night_id].offset;
		length = iq->binaries[iq->entries[iq->id].night_id].length;
	}
	buffer = isp_plugin_handle(iq->owner_id, RTS_ISP_PLUG_TYPE_IQ);
	if (!buffer) {
		return -RTS_ISP_ENOTREADY;
	}
	buffer += offset;
	stream = pb_istream_from_buffer(buffer, length);
#if USE_PB_DECODE
	if (!pb_decode(&stream, isp_iq_table_t_fields, iq_table)) {
		ret = -RTS_ISP_EPROTOBUF;
		goto out;
	}
#else
#	include "amb3_iq_default_ret.h"
#	include "amb3_iq_default_ret_poolinfo.h"
	pool_update_ptr(__isp_ret__, __isp_ret_poolinfo__);
	iq_table = __isp_ret__;
#endif
	if (iq_table->header.magic != ISP_IQ_HEADER_MAGIC ||
		iq_table->header.version >> 16 != ISP_IQ_HEADER_VERSION >> 16) {
		isp_error("iq magic or version does not match");
		ret = -RTS_ISP_EINVAL;
		goto out;
	}
	if (iq_table->extended_info.content &&
		iq_table->extended_info.content->size > 1024) {
		isp_error("iq extend info is too large");
		ret = -RTS_ISP_ERANGE;
		goto out;
	}
	ret = RTS_ISP_OK;
	tmp = iq->iq_table;
	iq->iq_table = iq_table;
	iq_table = tmp;
out:
	pb_release(isp_iq_table_t_fields, iq_table);
	isp_free(iq_table);
	if (ret) {
		isp_perror(ret, "load iq file fail");
	}
	return ret;
}

int isp_iq_change_table(struct isp_iq *iq, int id)
{
	int ret;
	int last_id;

	if (!iq || id >= _MAX_ISP_IQ_NUM || !isp_test_bit(iq->entry_bits, id)) {
		return -RTS_ISP_EINVAL;
	}
	if (iq->id == id) {
		return RTS_ISP_OK;
	}

	last_id = iq->id;
	iq->id = id;
	ret = isp_iq_load_from_shmem(iq);
	if (ret) {
		iq->id = last_id;
	}
	return ret;
}

int isp_iq_change_day_night(struct isp_iq *iq, int dn)
{
	int ret;
	int last_dn;

	if (!iq || dn < RTS_ISP_DAY || dn > RTS_ISP_NIGHT) {
		return -RTS_ISP_EINVAL;
	}
	if (iq->dn == dn) {
		return RTS_ISP_OK;
	}

	last_dn = iq->dn;
	iq->dn = dn;
	ret = isp_iq_load_from_shmem(iq);
	if (ret) {
		iq->dn = last_dn;
	}
	return ret;
}

int isp_iq_set_mode(struct isp_iq *iq, int mode)
{
	if (!iq) {
		return -RTS_ISP_EINVAL;
	}
	if (mode != RTS_ISP_HDR_NONE && mode != RTS_ISP_HDR_LINE_2TO1 &&
		mode != RTS_ISP_HDR_DCG_2TO1 && mode != RTS_ISP_HDR_DCG_COMBINE &&
		mode != RTS_ISP_HDR_DCG_COMBINE_TM) {
		return -RTS_ISP_EINVAL;
	}
	iq->mode = mode;
	return RTS_ISP_OK;
}

int isp_iq_set_scene(struct isp_iq *iq, int scene)
{
	if (!iq) {
		return -RTS_ISP_EINVAL;
	}
	if (scene < RTS_ISP_SCENE_NORMAL || scene > RTS_ISP_SCENE_CUSTOM7) {
		return -RTS_ISP_EINVAL;
	}
	iq->scene = scene;
	iq->scene_has_set = RTS_ISP_TRUE;
	return RTS_ISP_OK;
}

int isp_iq_set_outdoor(struct isp_iq *iq, int outdoor)
{
	if (!iq) {
		return -RTS_ISP_EINVAL;
	}
	iq->outdoor = !!outdoor;
	return RTS_ISP_OK;
}

int isp_iq_set_high_temp(struct isp_iq *iq, int high_temp)
{
	if (!iq) {
		return -RTS_ISP_EINVAL;
	}
	iq->high_temp = !!high_temp;
	return RTS_ISP_OK;
}

int isp_iq_get_ctrl_info(struct isp_iq *iq, struct isp_iq_ctrl_info *info)
{
	int i;

	if (!iq || !info) {
		return -RTS_ISP_EINVAL;
	}

	info->id = iq->id;
	info->entry_bits = iq->entry_bits;
	for (i = 0; i < _MAX_ISP_IQ_NUM; i++)
		if (iq->entry_bits & (1 << i)) {
			info->names[i] = iq->entries[i].name;
		} else {
			info->names[i] = "None";
		}

	return RTS_ISP_OK;
}

isp_algo_param_t *isp_iq_get_algo_param(uint32_t isp_id)
{
	struct isp_iq *iq;

	iq = isp_core_get_iq(isp_top_get_core(isp_id));
	if (!iq) {
		return NULL;
	}
	return &iq->algo_param;
}

isp_algo_param_header_t *isp_iq_get_algo_param_header(uint32_t isp_id)
{
	struct isp_iq *iq;

	iq = isp_core_get_iq(isp_top_get_core(isp_id));
	if (!iq) {
		return NULL;
	}
	return &iq->algo_param.header;
}

int isp_iq_current_table(uint32_t isp_id)
{
	struct isp_iq *iq;

	iq = isp_core_get_iq(isp_top_get_core(isp_id));
	if (!iq) {
		return 0;
	}
	return iq->id;
}

int isp_iq_is_night(uint32_t isp_id)
{
	struct isp_iq *iq;

	iq = isp_core_get_iq(isp_top_get_core(isp_id));
	if (!iq) {
		return RTS_ISP_FALSE;
	}
	return iq->dn == RTS_ISP_NIGHT;
}

const isp_iq_header_t *isp_iq_get_header(uint32_t isp_id)
{
	struct isp_iq *iq;

	iq = isp_core_get_iq(isp_top_get_core(isp_id));
	if (!iq) {
		return NULL;
	}
	if (!iq->iq_table || !iq->iq_table->header.magic) {
		return &default_iq.header;
	}
	return &iq->iq_table->header;
}

static int isp_iq_bound_is_group(int type)
{
	return (type / 16) & 1;
}

static int isp_iq_group_bounds_check(const isp_iq_bounds_t *bounds,
									 uint32_t count)
{
	int i;

	if (bounds->bounds_count + 1 != count ||
		bounds->def_index > bounds->bounds_count) {
		return -RTS_ISP_EINVAL;
	}
	for (i = 0; i < bounds->bounds_count; i++)
		if (bounds->bounds[i].th0 < bounds->bounds[i].th1) {
			return -RTS_ISP_EINVAL;
		}
	for (i = 0; i < (int)(bounds->bounds_count - 1); i++)
		if (bounds->bounds[i].th0 >= bounds->bounds[i + 1].th1) {
			return -RTS_ISP_EINVAL;
		}
	return RTS_ISP_OK;
}

static int isp_iq_intp_bounds_check(const isp_iq_bounds_t *bounds,
									uint32_t count)
{
	int i;

	if (!bounds->bounds_count || bounds->bounds_count != count ||
		bounds->def_index >= bounds->bounds_count) {
		return -RTS_ISP_EINVAL;
	}
	for (i = 0; i < (int)(bounds->bounds_count - 1); i++)
		if (bounds->bounds[i].th0 >= bounds->bounds[i + 1].th0) {
			return -RTS_ISP_EINVAL;
		}
	return RTS_ISP_OK;
}

static int isp_iq_bounds_check(const isp_iq_bounds_t *bounds, uint32_t count)
{
	if (!bounds) {
		return -RTS_ISP_EINVAL;
	}

	if (isp_iq_bound_is_group(bounds->type)) {
		return isp_iq_group_bounds_check(bounds, count);
	} else {
		return isp_iq_intp_bounds_check(bounds, count);
	}
}

static int isp_iq_get_dyn_value(int type,
								const struct isp_notify_dynamic *dynamic)
{
	int index;

	if (!dynamic) {
		return 0;
	}
	index = type & 0xf;
	if (index > 3) {
		return 0;
	}

	if (type >= ISP_IQ_BOUND_EXP0_INTP && type <= ISP_IQ_BOUND_EXP3_GRP) {
		return dynamic->ae.exposure[index];
	} else if (type >= ISP_IQ_BOUND_GAIN0_INTP &&
			   type <= ISP_IQ_BOUND_GAIN3_GRP) {
		return dynamic->ae.iq_total_gain[index];
	} else if (type >= ISP_IQ_BOUND_EXP_GAIN0_INTP &&
			   type <= ISP_IQ_BOUND_EXP_GAIN3_GRP) {
		return dynamic->ae.iq_exp_gain[index];
	} else if (type >= ISP_IQ_BOUND_RATIO_INTP &&
			   type <= ISP_IQ_BOUND_RATIO2_GRP) {
		return dynamic->ae.iq_ratio[index];
	} else if (type >= ISP_IQ_BOUND_SNR_GAIN0_INTP &&
			   type <= ISP_IQ_BOUND_SNR_GAIN3_GRP) {
		return dynamic->ae.iq_sensor_gain[index];
	} else if (type >= ISP_IQ_BOUND_COLOR_TEMP_INTP &&
			   type <= ISP_IQ_BOUND_COLOR_TEMP_GRP) {
		return dynamic->awb.color_temp;
	} else if (type >= ISP_IQ_BOUND_HIGH_TEMP_INTP &&
			   type <= ISP_IQ_BOUND_HIGH_TEMP_GRP) {
		return dynamic->sensor.temperature;
	} else if (type >= ISP_IQ_BOUND_CONTRAST_INTP &&
			   type <= ISP_IQ_BOUND_CONTRAST_GRP) {
		return dynamic->ae.contrast;
	} else if (type >= ISP_IQ_BOUND_APEX_BV_INTP &&
			   type <= ISP_IQ_BOUND_APEX_BV_GRP) {
		return dynamic->ae.iq_bv;
	} else {
		return 0;
	}
}

static int isp_iq_get_intp_params(uint8_t indexes[2], int *pdyn_val,
								  int *ppre_dyn_val, int thds[2],
								  const isp_iq_bounds_t *bounds,
								  const struct isp_notify_dynamic *dynamic,
								  int (*bounds_active_cb)(int type, void *priv),
								  void *priv)
{
	int index;
	int dyn_val;
	int pre_dyn_val;
	int get_def = 0;

	if (dynamic) {
		dyn_val = isp_iq_get_dyn_value(bounds->type, dynamic);
		pre_dyn_val = isp_iq_get_dyn_value(bounds->type, dynamic->pre);
		if (!bounds->enable ||
			(bounds_active_cb && !bounds_active_cb(bounds->type, priv))) {
			get_def = 1;
		}
	} else {
		dyn_val = pre_dyn_val = 0;
		get_def = 1;
	}
	*pdyn_val = dyn_val;
	*ppre_dyn_val = pre_dyn_val;

	if (get_def) {
		indexes[0] = indexes[1] = bounds->def_index;
	} else {
		if (isp_iq_bound_is_group(bounds->type)) {
			index = indexes[0];
			if (index >= bounds->bounds_count) {
				index = bounds->bounds_count;
			}
			while (index < bounds->bounds_count &&
				   dyn_val >= bounds->bounds[index].th0) {
				index++;
			}
			while (index > 0 &&
				   dyn_val < bounds->bounds[index - 1].th1) {
				index--;
			}
			indexes[0] = indexes[1] = index;
		} else {
			for (index = 0; index < bounds->bounds_count; index++)
				if (dyn_val <= bounds->bounds[index].th0) {
					break;
				}
			if (index == 0) {
				indexes[0] = indexes[1] = 0;
			} else if (index == bounds->bounds_count) {
				indexes[0] = indexes[1] = index - 1;
			} else if (dyn_val == bounds->bounds[index].th0) {
				indexes[0] = indexes[1] = index;
			} else {
				indexes[0] = index - 1;
				indexes[1] = index;
				thds[0] = bounds->bounds[indexes[0]].th0;
				thds[1] = bounds->bounds[indexes[1]].th0;
			}
		}
	}
	return 0;
}

static int isp_iq_get_match_score(struct isp_iq *iq, int mode, int scene)
{
	int mode_score;
	int scene_score;

	if (mode == iq->mode) {
		mode_score = 1000;
	} else {
		mode_score = 0;
	}
	if (iq->high_temp && scene == ISP_IQ_SCENE_HIGH_TEMP) {
		scene_score = 100;
	} else if (iq->scene_has_set) {
		if (scene == iq->scene) {
			scene_score = 90;
		} else if (scene == ISP_IQ_SCENE_NORMAL) {
			scene_score = 10;
		} else {
			scene_score = 0;
		}
	} else {
		if (iq->outdoor && scene == ISP_IQ_SCENE_OUTDOOR) {
			scene_score = 90;
		} else if (scene == ISP_IQ_SCENE_NORMAL) {
			scene_score = 10;
		} else {
			scene_score = 0;
		}
	}

	return mode_score + scene_score;
}

#define isp_iq_check_dyn_nest_bounds(nest)                                    \
	({                                                                    \
		int i;                                                        \
		int ret;                                                      \
                                                                              \
		ret = isp_iq_bounds_check(&nest->bounds, nest->dyns_count);   \
		for (i = 0; ret == 0 && i < nest->dyns_count; i++)            \
			ret = isp_iq_bounds_check(&nest->dyns[i].bounds,      \
						  nest->dyns[i].items_count); \
		ret;                                                          \
	})

#define DEFINE_IQ_GET_ALGO_PARAM_FUNC(mod, ver)                             \
	isp_algo_param_##mod##_##ver##_t *isp_algo_param_get_##mod##_##ver( \
		uint32_t isp_id)                                            \
	{                                                                   \
		isp_algo_param_t *algo_param;                               \
                                                                            \
		algo_param = isp_iq_get_algo_param(isp_id);                 \
		if (!algo_param)                                            \
			return NULL;                                        \
		algo_param->mod.which_entry =                               \
			isp_algo_param_##mod##_t_##ver##_tag;               \
		return &algo_param->mod.entry.ver;                          \
	}

#define DEFINE_IQ_GET_IQ_FUNC(mod, ver)                                       \
	static const isp_iq_##mod##_##ver##_t *isp_iq_get_##mod##_##ver##_iq( \
		uint32_t isp_id, isp_algo_param_common_t *common)             \
	{                                                                     \
		int i;                                                        \
		int score;                                                    \
		int max_score = 0;                                            \
		struct isp_iq *iq;                                            \
		const isp_iq_table_t *iqt;                                    \
		const isp_iq_##mod##_t *result = NULL;                        \
                                                                              \
		if (!common)                                                  \
			return NULL;                                          \
		iq = isp_core_get_iq(isp_top_get_core(isp_id));               \
		if (!iq)                                                      \
			return NULL;                                          \
		if (!iq->iq_table || !iq->iq_table->mod.entries_count)        \
			iqt = &default_iq;                                    \
		else                                                          \
			iqt = iq->iq_table;                                   \
                                                                              \
		for (i = 0; i < iqt->mod.entries_count; i++) {                \
			const isp_iq_##mod##_t *entry = &iqt->mod.entries[i]; \
			if (entry->which_entry ==                             \
			    isp_iq_##mod##_t_##ver##_tag) {                   \
				score = isp_iq_get_match_score(               \
					iq, entry->desc.mode,                 \
					entry->desc.scene);                   \
				if (score > max_score) {                      \
					max_score = score;                    \
					result = entry;                       \
				}                                             \
			}                                                     \
		}                                                             \
		if (!result)                                                  \
			return NULL;                                          \
		isp_strlcpy(common->name, result->desc.name,                  \
			    sizeof(common->name));                            \
                                                                              \
		return &result->entry.ver;                                    \
	}

#define DEFINE_IQ_GET_FUNC(mod, ver)                                \
	DEFINE_IQ_GET_IQ_FUNC(mod, ver)                             \
	const isp_iq_##mod##_##ver##_t *isp_iq_get_##mod##_##ver(   \
		uint32_t isp_id, isp_algo_param_common_t *common)   \
	{                                                           \
		isp_algo_param_index_t *index;                      \
		const isp_iq_##mod##_##ver##_dyn_t *dyn;            \
		const isp_iq_##mod##_##ver##_dyn_nest_t *nest;      \
		const isp_iq_##mod##_##ver##_t *iq;                 \
                                                                    \
		iq = isp_iq_get_##mod##_##ver##_iq(isp_id, common); \
		if (!iq)                                            \
			return NULL;                                \
                                                                    \
		index = &common->index;                             \
		nest = &iq->dynamic.nest;                           \
		dyn = &nest->dyns[nest->bounds.def_index];          \
                                                                    \
		if (isp_iq_check_dyn_nest_bounds(nest))             \
			return NULL;                                \
                                                                    \
		index->indexes[0] = index->indexes[1] =             \
			nest->bounds.def_index;                     \
		index->indexes0[0] = index->indexes0[1] =           \
			dyn->bounds.def_index;                      \
		index->indexes1[0] = index->indexes1[1] =           \
			dyn->bounds.def_index;                      \
                                                                    \
		return iq;                                          \
	}

#define DEFINE_IQ_NON_DYN_GET_FUNC(mod, ver)                          \
	DEFINE_IQ_GET_IQ_FUNC(mod, ver)                               \
	const isp_iq_##mod##_##ver##_t *isp_iq_get_##mod##_##ver(     \
		uint32_t isp_id, isp_algo_param_common_t *common)     \
	{                                                             \
		return isp_iq_get_##mod##_##ver##_iq(isp_id, common); \
	}

#define DEFINE_IQ_GET_DYN_ITEM_FUNC_EX(mod, ver)                             \
	const isp_iq_##mod##_##ver##_dyn_item_t                              \
		*isp_iq_get_##mod##_##ver##_dyn_item_ex(                     \
			const isp_iq_##mod##_##ver##_dyn_nest_t *nest,       \
			isp_algo_param_common_t *common,                     \
			const struct isp_notify_dynamic *dynamic,            \
			const isp_iq_##mod##_##ver##_dyn_item_t *(           \
				*intp_iq_item_cb)(                           \
				int dyn_vals[3], int thds[6],                \
				const isp_iq_##mod##_##ver##_dyn_item_t      \
					*items[4],                           \
				int changed[3], void *intp_priv),            \
			void *intp_priv,                                     \
			int (*bounds_active_cb)(int bounds_type,             \
						void *bounds_priv),          \
			void *bounds_priv)                                   \
	{                                                                    \
		int ret;                                                     \
		uint8_t *indexes;                                            \
		uint8_t *indexes0;                                           \
		uint8_t *indexes1;                                           \
		isp_algo_param_index_t algo_index;                           \
		const isp_iq_##mod##_##ver##_dyn_t *dyns;                    \
		int dyn_vals[3] = {};                                        \
		int thds[6] = {};                                            \
		const isp_iq_##mod##_##ver##_dyn_item_t *items[4];           \
		int changed[3] = {};                                         \
		int pre_dyn_vals[3] = {};                                    \
		const isp_iq_##mod##_##ver##_dyn_item_t *pitem = NULL;       \
                                                                             \
		if (!nest || !common || (dynamic && !intp_iq_item_cb))       \
			goto out;                                            \
                                                                             \
		algo_index = common->index;                                  \
		indexes = algo_index.indexes;                                \
		indexes0 = algo_index.indexes0;                              \
		indexes1 = algo_index.indexes1;                              \
                                                                             \
		ret = isp_iq_get_intp_params(indexes, &dyn_vals[0],          \
					     &pre_dyn_vals[0], &thds[0],     \
					     &nest->bounds, dynamic,         \
					     bounds_active_cb, bounds_priv); \
		if (ret)                                                     \
			goto out;                                            \
		changed[0] = dyn_vals[0] != pre_dyn_vals[0];                 \
		dyns = &nest->dyns[indexes[0]];                              \
		ret = isp_iq_get_intp_params(indexes0, &dyn_vals[1],         \
					     &pre_dyn_vals[1], &thds[2],     \
					     &dyns->bounds, dynamic,         \
					     bounds_active_cb, bounds_priv); \
		if (ret)                                                     \
			goto out;                                            \
		changed[1] = dyn_vals[1] != pre_dyn_vals[1];                 \
		items[0] = &dyns->items[indexes0[0]];                        \
		if (indexes0[0] != indexes0[1]) {                            \
			items[1] = &dyns->items[indexes0[1]];                \
		} else {                                                     \
			items[1] = NULL;                                     \
		}                                                            \
		if (indexes[0] != indexes[1]) {                              \
			dyns = &nest->dyns[indexes[1]];                      \
			ret = isp_iq_get_intp_params(                        \
				indexes1, &dyn_vals[2], &pre_dyn_vals[2],    \
				&thds[4], &dyns->bounds, dynamic,            \
				bounds_active_cb, bounds_priv);              \
			if (ret)                                             \
				goto out;                                    \
			changed[2] = dyn_vals[2] != pre_dyn_vals[2];         \
			items[2] = &dyns->items[indexes1[0]];                \
			dyns = &nest->dyns[indexes[1]];                      \
			if (indexes1[0] != indexes1[1]) {                    \
				items[3] = &dyns->items[indexes1[1]];        \
			} else {                                             \
				items[3] = NULL;                             \
			}                                                    \
		} else {                                                     \
			indexes1[0] = indexes0[0];                           \
			indexes1[1] = indexes0[1];                           \
			items[2] = items[3] = NULL;                          \
			changed[2] = false;                                  \
		}                                                            \
		if (intp_iq_item_cb)                                         \
			pitem = intp_iq_item_cb(dyn_vals, thds, items,       \
						changed, intp_priv);         \
		else                                                         \
			pitem = items[0];                                    \
		common->index = algo_index;                                  \
out:                                                                         \
		return pitem;                                                \
	}                                                                    \
	int isp_iq_##mod##_##ver##_dyn_enabled(                              \
		const isp_iq_##mod##_##ver##_dyn_nest_t *nest)               \
	{                                                                    \
		if (!nest->bounds.enable &&                                  \
		    !nest->dyns[nest->bounds.def_index].bounds.enable)       \
			return RTS_ISP_FALSE;                                \
		return RTS_ISP_TRUE;                                         \
	}

#define DEFINE_IQ_GET_DYN_ITEM_FUNC(mod, ver)                              \
	DEFINE_IQ_GET_DYN_ITEM_FUNC_EX(mod, ver)                           \
	const isp_iq_##mod##_##ver##_dyn_item_t                            \
		*isp_iq_get_##mod##_##ver##_dyn_item(                      \
			const isp_iq_##mod##_##ver##_dyn_nest_t *nest,     \
			isp_algo_param_common_t *common,                   \
			const struct isp_notify_dynamic *dynamic,          \
			const isp_iq_##mod##_##ver##_dyn_item_t *(         \
				*intp_iq_item_cb)(                         \
				int dyn_vals[3], int thds[6],              \
				const isp_iq_##mod##_##ver##_dyn_item_t    \
					*items[4],                         \
				int changed[3], void *intp_priv),          \
			void *intp_priv)                                   \
	{                                                                  \
		return isp_iq_get_##mod##_##ver##_dyn_item_ex(             \
			nest, common, dynamic, intp_iq_item_cb, intp_priv, \
			NULL, NULL);                                       \
	}

#define DEFINE_IQ_GET_DYN_ITEM_DEFAULT_FUNC(mod, ver)                  \
	const isp_iq_##mod##_##ver##_dyn_item_t                        \
		*isp_iq_get_##mod##_##ver##_dyn_item_default(          \
			const isp_iq_##mod##_##ver##_dyn_nest_t *nest, \
			isp_algo_param_common_t *common)               \
	{                                                              \
		return isp_iq_get_##mod##_##ver##_dyn_item_ex(         \
			nest, common, NULL, NULL, NULL, NULL, NULL);   \
	}

#define DEFINE_IQ_NON_DYN_COMMON_FUNCS(mod, ver) \
	DEFINE_IQ_GET_ALGO_PARAM_FUNC(mod, ver)  \
	DEFINE_IQ_NON_DYN_GET_FUNC(mod, ver)

#define DEFINE_IQ_COMMON_FUNCS(mod, ver)        \
	DEFINE_IQ_GET_ALGO_PARAM_FUNC(mod, ver) \
	DEFINE_IQ_GET_FUNC(mod, ver)            \
	DEFINE_IQ_GET_DYN_ITEM_FUNC(mod, ver)   \
	DEFINE_IQ_GET_DYN_ITEM_DEFAULT_FUNC(mod, ver)

DEFINE_IQ_COMMON_FUNCS(awb, v1)
DEFINE_IQ_COMMON_FUNCS(blc, v1)
DEFINE_IQ_COMMON_FUNCS(ccm, v1)
DEFINE_IQ_COMMON_FUNCS(gamma, v1)
DEFINE_IQ_COMMON_FUNCS(ygc, v1)
DEFINE_IQ_COMMON_FUNCS(uvtune, v1)
DEFINE_IQ_COMMON_FUNCS(spe, v1)
DEFINE_IQ_COMMON_FUNCS(ygamma, v1)
DEFINE_IQ_COMMON_FUNCS(eeh, v1)
DEFINE_IQ_COMMON_FUNCS(daynight, v1)

DEFINE_IQ_GET_ALGO_PARAM_FUNC(nlsc, v1)
DEFINE_IQ_GET_FUNC(nlsc, v1)
DEFINE_IQ_GET_DYN_ITEM_FUNC_EX(nlsc, v1)
DEFINE_IQ_GET_DYN_ITEM_DEFAULT_FUNC(nlsc, v1)

DEFINE_IQ_NON_DYN_COMMON_FUNCS(mlsc, v1)
DEFINE_IQ_NON_DYN_COMMON_FUNCS(ae, v1)
DEFINE_IQ_NON_DYN_COMMON_FUNCS(af, v1)
DEFINE_IQ_NON_DYN_COMMON_FUNCS(wdr, v1)
DEFINE_IQ_NON_DYN_COMMON_FUNCS(high_temp, v1)
DEFINE_IQ_NON_DYN_COMMON_FUNCS(flick, v1)

DEFINE_IQ_GET_IQ_FUNC(texture, v1)
const isp_iq_texture_v1_t *
isp_iq_get_texture_v1(uint32_t isp_id, isp_algo_param_common_t *common)
{
	int i;
	const isp_iq_texture_v1_t *iq;

	iq = isp_iq_get_texture_v1_iq(isp_id, common);
	if (!iq) {
		return iq;
	}

	for (i = 0; i < iq->dynamic.nests_count; i++) {
		const isp_iq_texture_v1_dyn_nest_t *nest;

		nest = &iq->dynamic.nests[i];
		if (isp_iq_check_dyn_nest_bounds(nest)) {
			return NULL;
		}
	}

	return iq;
}
DEFINE_IQ_GET_DYN_ITEM_FUNC(texture, v1)
DEFINE_IQ_GET_ALGO_PARAM_FUNC(texture, v1)
