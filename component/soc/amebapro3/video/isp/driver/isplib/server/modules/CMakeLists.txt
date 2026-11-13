set(module_files
    isp_mod_ae.c
    isp_mod_af.c
    isp_mod_awb.c
    isp_mod_blc.c
    isp_mod_ccm.c
    isp_mod_control.c
    isp_mod_crop.c
    isp_mod_dpc.c
    isp_mod_dpc_rnr.c
    isp_mod_eeh.c
    isp_mod_fbc_tnr.c
    isp_mod_fbc_vhdr.c
    isp_mod_flick.c
    isp_mod_fusion.c
    isp_mod_gamma.c
    isp_mod_global.c
    isp_mod_hue.c
    isp_mod_input.c
    isp_mod_interface.c
    isp_mod_intp.c
    isp_mod_iq.c
    isp_mod_lce.c
    isp_mod_ldc.c
    isp_mod_ltm.c
    isp_mod_mask.c
    isp_mod_mcrop.c
    isp_mod_mlsc.c
    isp_mod_nlsc.c
    isp_mod_rawstat.c
    isp_mod_rgb2yuv.c
    isp_mod_sensor.c
    isp_mod_spe.c
    isp_mod_statis.c
    isp_mod_tnr.c
    isp_mod_uvtune.c
    isp_mod_vctrl.c
    isp_mod_wdr.c
    isp_mod_ygamma.c
    isp_mod_ygc.c
    isp_mod_ystat.c
)
include(gen_modules_file.cmake)

set(source ${module_files} isp_modules.c)
set(target isp_server_modules)
add_library(${target} OBJECT ${source})
target_include_directories(${target} PRIVATE ${isptopdir}/common/core)
target_include_directories(${target} PRIVATE ${ispserverdir}/core)
target_include_directories(${target} PRIVATE ${ispserverdir}/hardware)
target_link_libraries(${target} PRIVATE isp_common_interface)
target_link_libraries(${target} PRIVATE isp_server_interface)

set_property(GLOBAL APPEND PROPERTY isp_server_objs $<TARGET_OBJECTS:${target}>)
