### add .cmkae need if neeeded ###
message(STATUS "build VIPLite Driver")

### add flags ###
list(
	APPEND viplite_flags
	#OPE_EXPORT=extern
)

### add header files ###
list (
	APPEND viplite_inc_path

	${CMAKE_CURRENT_LIST_DIR}/nbg_linker/inc
	${CMAKE_CURRENT_LIST_DIR}/vip_drv/inc
	${CMAKE_CURRENT_LIST_DIR}/vip_drv/src
	${CMAKE_CURRENT_LIST_DIR}/vip_drv/src/task
	${CMAKE_CURRENT_LIST_DIR}/vip_drv/src/memory
	${CMAKE_CURRENT_LIST_DIR}/vip_hal/src
	${CMAKE_CURRENT_LIST_DIR}/vip_hal/inc
	${CMAKE_CURRENT_LIST_DIR}/include
	${CMAKE_CURRENT_LIST_DIR}/include/nbg_linker
	${CMAKE_CURRENT_LIST_DIR}/common
	${CMAKE_CURRENT_LIST_DIR}/vip_drv/src/os/freeRTOS
	${CMAKE_CURRENT_LIST_DIR}/vip_hal/src/os/freeRTOS
)

### add source file ###
list(
	APPEND viplite_src

	${CMAKE_CURRENT_LIST_DIR}/nbg_linker/nbg_lk_cpu.c
	${CMAKE_CURRENT_LIST_DIR}/nbg_linker/nbg_lk_user_cpu.c
	${CMAKE_CURRENT_LIST_DIR}/nbg_linker/nbg_fmt_util.c
	${CMAKE_CURRENT_LIST_DIR}/nbg_linker/nbg_fmt_v1.c
	${CMAKE_CURRENT_LIST_DIR}/nbg_linker/nbg_fmt_v2.c
	${CMAKE_CURRENT_LIST_DIR}/nbg_linker/nbg_lk_relocate.c
	${CMAKE_CURRENT_LIST_DIR}/nbg_linker/nbg_lk.c
	${CMAKE_CURRENT_LIST_DIR}/nbg_linker/nbg_lk_buffer.c
	${CMAKE_CURRENT_LIST_DIR}/nbg_linker/nbg_lk_coeff.c
	${CMAKE_CURRENT_LIST_DIR}/nbg_linker/nbg_lk_debug.c
	${CMAKE_CURRENT_LIST_DIR}/nbg_linker/nbg_lk_network.c
	${CMAKE_CURRENT_LIST_DIR}/nbg_linker/nbg_lk_group.c
	${CMAKE_CURRENT_LIST_DIR}/nbg_linker/nbg_lk_util.c
	${CMAKE_CURRENT_LIST_DIR}/nbg_linker/nbg_lk_context.c
	${CMAKE_CURRENT_LIST_DIR}/nbg_linker/nbg_lk_capture.c
	${CMAKE_CURRENT_LIST_DIR}/nbg_linker/nbg_lk_npd.c
	${CMAKE_CURRENT_LIST_DIR}/nbg_linker/nbg_lk_nld.c
	${CMAKE_CURRENT_LIST_DIR}/nbg_linker/nbg_lk_list.c
	${CMAKE_CURRENT_LIST_DIR}/vip_hal/src/vip_hal_hardware.c
	${CMAKE_CURRENT_LIST_DIR}/vip_drv/src/vip_drv_hardware.c
	${CMAKE_CURRENT_LIST_DIR}/vip_drv/src/vip_drv_device.c
	${CMAKE_CURRENT_LIST_DIR}/vip_drv/src/vip_drv_context.c
	${CMAKE_CURRENT_LIST_DIR}/vip_drv/src/vip_drv_interface.c
	${CMAKE_CURRENT_LIST_DIR}/vip_drv/src/vip_drv_util.c
	${CMAKE_CURRENT_LIST_DIR}/vip_drv/src/vip_drv_pm.c
	${CMAKE_CURRENT_LIST_DIR}/vip_drv/src/memory/vip_drv_video_memory.c
	${CMAKE_CURRENT_LIST_DIR}/vip_drv/src/task/vip_drv_task_descriptor.c
	${CMAKE_CURRENT_LIST_DIR}/vip_drv/src/task/vip_drv_task_schedule.c
	# ${CMAKE_CURRENT_LIST_DIR}/vip_drv/src/task/vip_drv_task_parallel_async.c
	# ${CMAKE_CURRENT_LIST_DIR}/vip_drv/src/task/vip_drv_task_serial_sync.c
	# ${CMAKE_CURRENT_LIST_DIR}/vip_drv/src/task/vip_drv_task_parallel_sync.c
	${CMAKE_CURRENT_LIST_DIR}/vip_drv/src/task/vip_drv_task_single.c
	${CMAKE_CURRENT_LIST_DIR}/vip_drv/src/task/vip_drv_task_common.c
	${CMAKE_CURRENT_LIST_DIR}/vip_drv/src/task/vip_drv_task_debug.c
	${CMAKE_CURRENT_LIST_DIR}/vip_drv/src/memory/vip_drv_mmu.c
	${CMAKE_CURRENT_LIST_DIR}/vip_drv/src/memory/vip_drv_mem_heap.c
	${CMAKE_CURRENT_LIST_DIR}/vip_drv/src/vip_drv_debug.c
	${CMAKE_CURRENT_LIST_DIR}/vip_drv/src/os/freeRTOS/vip_drv_os.c
	${CMAKE_CURRENT_LIST_DIR}/vip_drv/src/os/freeRTOS/vip_drv_device_driver.c
	${CMAKE_CURRENT_LIST_DIR}/vip_drv/src/memory/vip_drv_mem_allocator.c
	${CMAKE_CURRENT_LIST_DIR}/vip_drv/src/os/freeRTOS/vip_drv_mem_allocator_heap.c
	${CMAKE_CURRENT_LIST_DIR}/vip_drv/src/os/freeRTOS/vip_drv_mem_allocator_continue.c
	${CMAKE_CURRENT_LIST_DIR}/vip_drv/src/os/freeRTOS/vip_drv_mem_allocator_wrap_memory.c
	${CMAKE_CURRENT_LIST_DIR}/vip_drv/src/os/freeRTOS/vip_drv_mem_allocator_wrap_physical.c
	${CMAKE_CURRENT_LIST_DIR}/vip_hal/src/vip_hal.c
	${CMAKE_CURRENT_LIST_DIR}/vip_hal/src/vip_hal_hardware.c
	${CMAKE_CURRENT_LIST_DIR}/vip_hal/src/vip_hal_heap.c
	${CMAKE_CURRENT_LIST_DIR}/vip_hal/src/os/freeRTOS/vip_hal_os.c
)

#message(STATUS "VIPLITE source ${viplite_src}")
#message(STATUS "VIPLITE include path ${viplite_inc_path}")