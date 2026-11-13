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

##########################################################################
# File Name: drive_cmake.sh
# Author: Arthur Lee
# mail: Xuanji.Li@verisilicon.com
# Created Time: Thu 26 Nov 2020 03:54:27 PM CST
#########################################################################
#!/bin/bash
set -x
set -e

echo $FREERTOS_DIR
PWD=`pwd`
if [ ! $1 ]
then
    if [ ! $FREERTOS_DIR ]
    then
        FREERTOS_DIR=$PWD/../../../../software/../../../FreeRTOS
    fi
else
    FREERTOS_DIR=$1    
fi

rm -rf CMakeCache.txt CMakeFiles cmake_install.cmake Makefile *.a *.so
cmake ./ -DFREERTOS_DIR_PATH:STRING=$FREERTOS_DIR

make -f Makefile
