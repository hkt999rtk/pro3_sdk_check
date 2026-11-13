#include <rtsavisp.h>
#include <isp_os_mem.h>

static inline int algo_id_to_plug_id(int algo_id)
{
	return algo_id;
}

static inline int check_algo_id(int algo_id)
{
	if (algo_id < RTS_ISP_ALGO_AE_ID0 || algo_id > RTS_ISP_ALGO_OTHER_ID3) {
		return -RTS_ISP_EINVAL;
	}
	return RTS_ISP_OK;
}

int rts_av_isp_register_algo(const struct rts_isp_algo *algo)
{
	struct rts_isp_plugin plug;

	if (!algo || check_algo_id(algo->id) || !algo->path) {
		return -RTS_ISP_EINVAL;
	}

	plug.id = algo_id_to_plug_id(algo->id);
	plug.bin_type = RTS_ISP_PLUG_BIN_TYPE_FILE;
	plug.path = algo->path;
	return rts_av_isp_register_plugin(&plug);
}

int rts_av_isp_unregister_algo(int32_t algo_id)
{
	if (check_algo_id(algo_id)) {
		return -RTS_ISP_EINVAL;
	}
	return rts_av_isp_unregister_plugin(algo_id_to_plug_id(algo_id));
}

int rts_av_isp_get_algo(struct rts_isp_algo *algo)
{
	int ret;
	struct rts_isp_plugin plug;

	if (!algo || check_algo_id(algo->id)) {
		return -RTS_ISP_EINVAL;
	}
	plug.id = algo_id_to_plug_id(algo->id);
	ret = rts_av_isp_get_plugin(&plug);
	if (ret) {
		return ret;
	}
	algo->path = plug.path;

	return RTS_ISP_OK;
}

int rts_av_isp_bind_algo(uint32_t isp_id, int32_t algo_id)
{
	if (isp_id > _ISP_ID_NUM || check_algo_id(algo_id)) {
		return -RTS_ISP_EINVAL;
	}
	return rts_av_isp_bind_plugin(isp_id, algo_id_to_plug_id(algo_id));
}

int rts_av_isp_unbind_algo(uint32_t isp_id, int32_t algo_id)
{
	if (isp_id > _ISP_ID_NUM || check_algo_id(algo_id)) {
		return -RTS_ISP_EINVAL;
	}
	return rts_av_isp_bind_plugin(isp_id, algo_id_to_plug_id(algo_id));
}

static inline int check_sensor_id(int sensor_id)
{
	if (sensor_id < RTS_ISP_PLUG_SENSOR_ID_MIN ||
		sensor_id > RTS_ISP_PLUG_SENSOR_ID_MAX) {
		return -RTS_ISP_EINVAL;
	}
	return RTS_ISP_OK;
}

int rts_av_isp_register_sensor(const struct rts_isp_sensor *sensor)
{
	int id;
	int ret;
	struct rts_isp_plugin plug;

	for (id = RTS_ISP_PLUG_SENSOR_ID_MIN; id <= RTS_ISP_PLUG_SENSOR_ID_MAX;
		 id++) {
		plug.id = id;
		ret = rts_av_isp_get_plugin(&plug);
		if (ret) {
			plug.bin_type = RTS_ISP_PLUG_BIN_TYPE_FILE;
			plug.path = sensor->path;
			ret = rts_av_isp_register_plugin(&plug);
			if (ret) {
				return ret;
			}
			return id;
		}
	}
	return -RTS_ISP_ENOMEM;
}

int rts_av_isp_unregister_sensor(int sensor_id)
{
	if (check_sensor_id(sensor_id)) {
		return -RTS_ISP_EINVAL;
	}

	return rts_av_isp_unregister_plugin(sensor_id);
}

int rts_av_isp_get_sensor(int sensor_id, struct rts_isp_sensor *sensor)
{
	int ret;
	struct rts_isp_plugin plug;

	if (check_sensor_id(sensor_id)) {
		return -RTS_ISP_EINVAL;
	}

	plug.id = sensor_id;
	ret = rts_av_isp_get_plugin(&plug);
	if (ret) {
		return ret;
	}
	sensor->path = plug.path;

	return RTS_ISP_OK;
}

int rts_av_isp_check_sensor(uint32_t isp_id, int sensor_id)
{
	if (isp_id > _ISP_ID_NUM || check_sensor_id(sensor_id)) {
		return -RTS_ISP_EINVAL;
	}
	return rts_av_isp_check_plugin(isp_id, sensor_id);
}

int rts_av_isp_bind_sensor(uint32_t isp_id, int sensor_id)
{
	if (isp_id > _ISP_ID_NUM || check_sensor_id(sensor_id)) {
		return -RTS_ISP_EINVAL;
	}
	return rts_av_isp_bind_plugin(isp_id, sensor_id);
}

int rts_av_isp_unbind_sensor(uint32_t isp_id, int sensor_id)
{
	if (isp_id > _ISP_ID_NUM || check_sensor_id(sensor_id)) {
		return -RTS_ISP_EINVAL;
	}
	return rts_av_isp_unbind_plugin(isp_id, sensor_id);
}

int rts_av_isp_register_iq(uint32_t isp_id, const char *path)
{
	int id;
	int ret;
	struct rts_isp_plugin plug;

	if (isp_id > _ISP_ID_NUM || !path) {
		return -RTS_ISP_EINVAL;
	}

	for (id = RTS_ISP_PLUG_IQ_ID_MIN; id <= RTS_ISP_PLUG_IQ_ID_MAX; id++) {
		plug.id = id;
		ret = rts_av_isp_get_plugin(&plug);
		if (!ret && !isp_strcmp(plug.path, path)) {
			return rts_av_isp_bind_plugin(isp_id, id);
		}
	}

	for (id = RTS_ISP_PLUG_IQ_ID_MIN; id <= RTS_ISP_PLUG_IQ_ID_MAX; id++) {
		plug.id = id;
		ret = rts_av_isp_get_plugin(&plug);
		if (ret) {
			plug.bin_type = RTS_ISP_PLUG_BIN_TYPE_FILE;
			plug.path = path;
			ret = rts_av_isp_register_plugin(&plug);
			if (ret) {
				return ret;
			}
			ret = rts_av_isp_bind_plugin(isp_id, id);
			if (ret) {
				rts_av_isp_unregister_plugin(id);
			}
			return ret;
		}
	}

	return -RTS_ISP_ENOMEM;
}
