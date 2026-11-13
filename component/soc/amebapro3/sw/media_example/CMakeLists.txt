
if (CONFIG_ARM_CORE_CA32)

if (c_ENABLE_EXAMPLE)
	ameba_add_subdirectory(ap_media_example)
endif()

elseif(CONFIG_ARM_CORE_CM4_VP OR CONFIG_PXP)
	ameba_add_subdirectory_if(CONFIG_VP_MEDIA_EXAMPLE vp_media_example)
endif()
