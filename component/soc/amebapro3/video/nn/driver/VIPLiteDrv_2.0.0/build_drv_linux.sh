#!/bin/bash

########################################################
# establish build environment and build options value
# Please modify the following items according your build environment

BUILD_BOARD=X86_PCIE


boardArray=(X86_PCIE X86_CMODEL vim3 toybrick)

export VIPLITE_ROOT=`pwd`
export SDK_DIR=$VIPLITE_ROOT/build/sdk


OPT_ENABLE_NODE=0
USE_LINUX_CMA=0
USE_LINUX_PLATFORM_DEVICE=0
USE_LINUX_PCIE_DEVICE=0
FPGA_BUILD=0
OPT_ENABLE_ASAN=0
OPT_BUILD_DEBUG=0
BUILDTYPE=release

usage() {
cat << EOF
usage: ./build.sh [-h:s:] BOARD BUILDTYPE clean/install

    -s: build static library
    -p: build preprocess node. only for sclaer engine now.
    -m: enable memory leak check with asan
    -e: used reserved memory for video memory heap
    -u: dissble VIP's MMU engine
    -d: build debug driver
    -l: list BOARD option
    -h: help

    BUILDTYPE: debug / release
    BOARD: X86_PCIE (your build target)

    example:
    ./build_drv.sh                          #default target build release version
    ./build_drv.sh clean                    #default target clean
    ./build_drv.sh install                  #default target build and install release version
    ./build_drv.sh X86_PCIE install         #X86_PCIE target release version
    ./build_drv.sh X86_PCIE release clean
    ./build_drv.sh X86_PCIE release clean install
    ./build_drv.sh X86_PCIE release install
    ./build_drv.sh -d install             #build debug version
    ./build_drv.sh X86_PCIE debug install
    ./build_drv.sh -s install               #build static library

Support build BOARD target list:
    ${boardArray[@]}
EOF
    exit 1
}

while getopts abcdkABCefgGilxhLmnspP23rRTtuvVS:E:N:X:Y:Z:D: opt
do
    case "$opt" in
    s)
        export STATIC_LINK_BUILD=1
    ;;
    p)
       OPT_ENABLE_NODE=1
    ;;
    d)
        OPT_BUILD_DEBUG=1
    ;;
    u)
        export ENABLE_MMU=0
    ;;
    m)
       OPT_ENABLE_ASAN=1
    ;;
    l)
       echo "Support build board target list:"
       echo ${boardArray[@]}
       exit 1
    ;;
    h)
        usage
    ;;
    *)
        echo "ERROR: unknown options: $opt"
        usage
        echo
        exit 1
    ;;
    esac
done


shift $[ $OPTIND - 1 ]

BOARD=$1
shift
BUILDTYPE=$1
shift

#use ./build_drv.sh install or clean to build command driver
if [ "$BOARD" = "clean" ]; then
    shift $#
    set -- ${BOARD}
    BOARD=$BUILD_BOARD
elif [ "$BOARD" = "install" ]; then
    shift $#
    set -- ${BOARD}
    BOARD=$BUILD_BOARD
fi


#default build release version
if [ -z $BUILDTYPE ]; then
    BUILDTYPE=release
elif [ "$BUILDTYPE" = "clean" ]; then
    shift $#
    set -- ${BUILDTYPE}
    BUILDTYPE=release
elif [ "$BUILDTYPE" = "install" ]; then
    shift $#
    set -- ${BUILDTYPE}
    BUILDTYPE=release
fi

case ${BUILDTYPE} in
    debug) DEBUG=1 ;;
    DEBUG) DEBUG=1 ;;
    release) DEBUG=0 ;;
    RELEASE) DEBUG=0 ;;
esac

#used default board to build driver
if [ -z $BOARD ]; then
BOARD=$BUILD_BOARD
fi

if [ "$OPT_BUILD_DEBUG" = "1" ]; then
    DEBUG=1
    BUILDTYPE=debug
fi

echo "BUILDTYPE"=$BUILDTYPE
echo "BOARD"=$BOARD

USEMAKEFILE=makefile.linux

#PLATFORM_NAME is specify platform code path. is vendor company name.
#SOC_CONFIG is board/poroject name.

case "$BOARD" in
toybrick)
    export ARCH_TYPE=arm64
    export CPU_TYPE=cortex-a53
    export CPU_ARCH=armv8-a
    export FIXED_ARCH_TYPE=arm64
    export KERNEL_DIR=/home/software/Linux/rockchips/toybrick_kernel
    export TOOLCHAIN=/home/software/Linux/rockchips/gcc-linaro-6.3.1-2017.05-x86_64_aarch64-linux-gnu/bin
    export CROSS_COMPILE=$TOOLCHAIN/aarch64-linux-gnu-
    export LIB_DIR=/home/software/Linux/rockchips/gcc-linaro-6.3.1-2017.05-x86_64_aarch64-linux-gnu
    export USE_LINUX_PLATFORM_DEVICE=1
    export PLATFORM_NAME=toybrick
    export AUTO_CORRECT_CONFLICTS=1
;;
vim3)
    export ARCH_TYPE=arm64
    export CPU_TYPE=cortex-a53
    export CPU_ARCH=armv8-a
    export FIXED_ARCH_TYPE=arm64
    export KERNEL_DIR=/home/software/Linux/VIM3/linux/
    export TOOLCHAIN=/home/software/Linux/amlogic/gcc-linaro-6.3.1-2017.05-x86_64_aarch64-linux-gnu/bin
    export CROSS_COMPILE=$TOOLCHAIN/aarch64-linux-gnu-
    export LIB_DIR=/home/software/Linux/amlogic/gcc-linaro-6.3.1-2017.05-x86_64_aarch64-linux-gnu/aarch64-linux-gnu/libc/lib
    #export USE_LINUX_CMA=1
    export USE_LINUX_PLATFORM_DEVICE=1
    export PLATFORM_NAME=amlogic
    export SOC_CONFIG=vim3
    export AUTO_CORRECT_CONFLICTS=1
;;
emulator-85)
    export ARCH_TYPE=x86_64
    export KERNEL_DIR=/vip_data_center/VIP_BSP/Emulator/emulator_kernel/linux-headers-4.15.0-20-generic
    export TOOLCHAIN=/usr/bin
    export CROSS_COMPILE=""
    export CPU_TYPE=0
    export CPU_ARCH=0
    export X11_ARM_DIR=/usr/X11R6
    export USE_LINUX_PCIE_DEVICE=1
    export FPGA_BUILD=1
    export PLATFORM_NAME=vivante
    export SOC_CONFIG=emulator
    export USE_PCIE_MSI=1
    export RESERVED_HEAP_NODE=0
    export AUTO_CORRECT_CONFLICTS=1
    export ENABLE_MMU=1
;;
X86_PCIE)
    export ARCH_TYPE=x86_64
    GCC_VERSION=$(eval "gcc -dumpversion")
    echo "gcc version=$GCC_VERSION"
    if [ $GCC_VERSION -gt "7" ]
    then
        export KERNEL_DIR=/home/software/Linux/x86_pcie/linux-6.1.12
    else
        export KERNEL_DIR=/home/software/Linux/x86_pcie/linux-headers-4.8.0-41-generic
    fi
    echo "KERNEL_DIR=$KERNEL_DIR"
    export TOOLCHAIN=/usr/bin
    export CROSS_COMPILE=""
    export CPU_TYPE=0
    export CPU_ARCH=0
    export X11_ARM_DIR=/usr/X11R6
    export USE_LINUX_PCIE_DEVICE=1
    export FPGA_BUILD=1
    export PLATFORM_NAME=vivante
    export SOC_CONFIG=pcie
    export AUTO_CORRECT_CONFLICTS=1
;;
X86_CMODEL)
    export AQROOT=${VIPLITE_ROOT}/vip_drv/src/os/linuxemulator/
    export AQARCH=${AQROOT}/cmodel_wrapper/arch/
    export ARCH_TYPE=x86_64
    export TOOLCHAIN=/usr
    export CROSS_COMPILE=""
    export CPU_TYPE=0
    export CPU_ARCH=0
    export PLATFORM_NAME=vivante
    export SOC_CONFIG=linux_emulator
    export LINUXEMULATOR=1
    USEMAKEFILE=makefile.linuxemulator
    CMODEL_LIB_PATH=${VIPLITE_ROOT}/vip_drv/src/os/linuxemulator/cmodel_wrapper/prebuild
    echo "AQROOT=$AQROOT"
    if [ -d "${UNIFIED_DRV_ROOT}" ]; then
        export LIBCMODEL_PREBUILD=0
        echo "build viplite from cmodel source code"
        mkdir -p ${AQROOT}/hal/inc/
        mkdir -p $AQARCH
        CMODEL_PATH=$UNIFIED_DRV_ROOT/arch/XAQ2/cmodel/
        REG_PATH=$UNIFIED_DRV_ROOT/arch/XAQ2/reg/
        TOOLS_PATH=$UNIFIED_DRV_ROOT/tools/
        echo "CMODEL_PATH=$CMODEL_PATH"
        echo "REG_PATH=$REG_PATH"
        cp -rfLR ${CMODEL_PATH} ${AQARCH}/
        cp -rfLR ${REG_PATH} ${AQARCH}/
        cp -r ${TOOLS_PATH}  ${AQROOT}/
    elif [ -e "$(ls ${CMODEL_LIB_PATH}/lib*)" ]; then
        export LIBCMODEL_PREBUILD=1
        echo "build viplite from cmodel binary"
    else
        echo "pls add prebuild cmodel library(libCmodelWrapper.so) into:${CMODEL_LIB_PATH}"
        echo "or"
        echo "pls copy cmodel/reg source code to ${AQROOT}/cmodel_wrapper/arch/"
        exit -1
    fi
;;
*)
   echo "ERROR: Unknown BOARD, or not support so far BOARD=$BOARD"
   usage
   exit 1
   ;;

esac;


########################################################
# set special build options valule
# You can modify the build options for different results according your requirement
#
#    option                           description                          default value
#    -------------------------------------------------------------------------------------
#    DEBUG                            Enable debugging.                               0
#                                     Disable debugging.
#
#    ABI                              Change application binary interface, default    0
#                                      is 0 which means no setting
#                                      aapcs-linux For example, build driver for Aspenite board
#
#    LINUX_OABI                       Enable this if build environment is ARM OABI.   0
#                                     Normally disable it for ARM EABI or other machines.
#
#    FPGA_BUILD                       To fix a pecical issue on FPGA board;           0
#
#    USE_LINUX_PCIE_DEVICE            Use linux PCIE driver                           0
#
#    USE_LINUX_CMA                    Allocate a memory from Linux's CMA as video memory heap  0
#

BUILD_OPTION_DEBUG=$DEBUG
BUILD_OPTION_ABI=0
BUILD_OPTION_LINUX_OABI=0

BUILD_OPTIONS="$BUILD_OPTIONS ABI=$BUILD_OPTION_ABI"
BUILD_OPTIONS="$BUILD_OPTIONS LINUX_OABI=$BUILD_OPTION_LINUX_OABI"
BUILD_OPTIONS="$BUILD_OPTIONS DEBUG=$BUILD_OPTION_DEBUG"
BUILD_OPTIONS="$BUILD_OPTIONS ENABLE_NODE=$OPT_ENABLE_NODE"
BUILD_OPTIONS="$BUILD_OPTIONS ENABLE_ASAN=$OPT_ENABLE_ASAN"

echo $PATH | grep "${TOOLCHAIN}" > /dev/null
if [ -n "${TOOLCHAIN}" -a \( $? -eq 1 \) ] ; then
    export PATH=$TOOLCHAIN:$PATH
fi


echo "BUILD_OPTIONS " $BUILD_OPTIONS

set -o pipefail
make -f ${USEMAKEFILE} $BUILD_OPTIONS $@ 2>&1 | tee $VIPLITE_ROOT/build_viplite.log
if [ $? -ne 0 ]
then
    echo "fail to build viplite driver..."
    exit 1
else
    echo "build viplite driver successfully..."
fi
set +o pipefail

exit 0
