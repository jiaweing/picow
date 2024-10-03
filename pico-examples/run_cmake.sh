#!/bin/bash
cd /Users/jiawei/Documents/SIT/picow/pico-examples
mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug \
      -DPICO_TOOLCHAIN_PATH=/opt/homebrew/bin \
      -DPICO_SDK_PATH=$PICO_SDK_PATH \
      -DFREERTOS_KERNEL_PATH=/Users/jiawei/Documents/FreeRTOS-KernelV11.1.0 \
      .. --log-level=VERBOSE > cmake_output.log 2>&1