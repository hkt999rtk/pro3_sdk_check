#!/bin/bash

rm build* -rf
rm sysroot/include sysroot/lib/firmware -rf
rm sysroot/lib/libdrivers.a sysroot/lib/libkernel.a sysroot/lib/libisp.a

mkdir buildkernel
cd buildkernel
cmake -DCMAKE_TOOLCHAIN_FILE=../toolchainfile.cmake ../kernel
make install
cd ..

mkdir builddrivers
cd builddrivers
cmake -DCMAKE_TOOLCHAIN_FILE=../toolchainfile.cmake ../drivers
make install
cd ..

mkdir buildisplib
cd buildisplib
cmake -DCMAKE_TOOLCHAIN_FILE=../toolchainfile.cmake ../isplib
make install
cd ..

mkdir buildfw
cd buildfw
cmake -DCMAKE_TOOLCHAIN_FILE=../toolchainfile.cmake ../apps
make install
cd ..
