#-------------------------------------------------------------------------------
#-                                                                            --
#-       This software is confidential and proprietary and may be used        --
#-        only as expressly authorized by a licensing agreement from          --
#-                                                                            --
#-                            VeriSilicon Inc.                                --
#-                                                                            --
#-                   (C) COPYRIGHT 2019 VeriSilicon Inc                       --
#-                            ALL RIGHTS RESERVED                             --
#-                                                                            --
#-                 The entire notice above must be reproduced                 --
#-                  on all copies and should not be removed.                  --
#-                                                                            --
#-------------------------------------------------------------------------------
###### For real FreeRTOS env with gcc toochian  #######
# Maybe the FreeRTOS Kernel has already been installed on the coreessponding hardware platform,
# so could still need the following header declare and c source code
FREERTOS_KERNEL_DIR = $(FREERTOS_DIR)/FreeRTOS_Kernel/Source
#should add the related portable for your platform
FREERTOS_INCLUDE_KERNEL = -I$(FREERTOS_KERNEL_DIR)/include \
           -I$(FREERTOS_DIR)/osal/portable/GCC/Posix \
           -I$(FREERTOS_DIR)/osal/portable/GCC/Posix/utils

#FreeRTOS kernel, should add the related portable for your platform
FREERTOS_KERNEL_C_SRCS = $(FREERTOS_KERNEL_DIR)/croutine.c \
                         $(FREERTOS_KERNEL_DIR)/event_groups.c \
                         $(FREERTOS_KERNEL_DIR)/list.c \
                         $(FREERTOS_KERNEL_DIR)/queue.c \
                         $(FREERTOS_KERNEL_DIR)/stream_buffer.c \
                         $(FREERTOS_KERNEL_DIR)/tasks.c \
                         $(FREERTOS_KERNEL_DIR)/timers.c \
                         $(FREERTOS_KERNEL_DIR)/portable/MemMang/heap_3.c \
                         $(FREERTOS_DIR)/osal/portable/GCC/Posix/port.c \
                         $(FREERTOS_DIR)/osal/portable/GCC/Posix/stub_function.c \
                         $(FREERTOS_DIR)/osal/portable/GCC/Posix/utils/wait_for_event.c

# list of used header files
# for FreeRTOS-Plus-POSIX/include/portable/pc/linux, should be revised according to the related platform
FREERTOS_POSIX_DIR = $(FREERTOS_DIR)/lib
FREERTOS_INCLUDE_POSIX = -I$(FREERTOS_POSIX_DIR)/FreeRTOS-Plus-POSIX/include/portable/pc/linux \
           -I$(FREERTOS_POSIX_DIR)/FreeRTOS-Plus-POSIX/include/portable \
           -I$(FREERTOS_POSIX_DIR)/FreeRTOS-Plus-POSIX/include \
           -I$(FREERTOS_POSIX_DIR)/include/private \
           -I$(FREERTOS_POSIX_DIR)/include \
           -I$(FREERTOS_POSIX_DIR)/include/FreeRTOS_POSIX

# Could contain header files about the file system, like FreeRTOS+FAT

# list of used source files
#FreeRTOS-POSIX Plus(pthread and ...)
FREERTOS_POSIX_SOURCE_DIR = $(FREERTOS_POSIX_DIR)/FreeRTOS-Plus-POSIX/source
FREERTOS_POSIX_C_SRCS = $(FREERTOS_POSIX_SOURCE_DIR)/FreeRTOS_POSIX_clock.c \
                        $(FREERTOS_POSIX_SOURCE_DIR)/FreeRTOS_POSIX_mqueue.c \
                        $(FREERTOS_POSIX_SOURCE_DIR)/FreeRTOS_POSIX_pthread.c \
                        $(FREERTOS_POSIX_SOURCE_DIR)/FreeRTOS_POSIX_pthread_barrier.c \
                        $(FREERTOS_POSIX_SOURCE_DIR)/FreeRTOS_POSIX_pthread_cond.c \
                        $(FREERTOS_POSIX_SOURCE_DIR)/FreeRTOS_POSIX_pthread_mutex.c \
                        $(FREERTOS_POSIX_SOURCE_DIR)/FreeRTOS_POSIX_sched.c \
                        $(FREERTOS_POSIX_SOURCE_DIR)/FreeRTOS_POSIX_semaphore.c \
                        $(FREERTOS_POSIX_SOURCE_DIR)/FreeRTOS_POSIX_timer.c \
                        $(FREERTOS_POSIX_SOURCE_DIR)/FreeRTOS_POSIX_unistd.c \
                        $(FREERTOS_POSIX_SOURCE_DIR)/FreeRTOS_POSIX_utils.c
# Could contain used source filesabout the file system, like FreeRTOS+FAT
FREERTOS_C_SRCS  = $(FREERTOS_KERNEL_C_SRCS) $(FREERTOS_POSIX_C_SRCS)
FREERTOS_INCLUDE = $(FREERTOS_INCLUDE_KERNEL) $(FREERTOS_INCLUDE_POSIX)