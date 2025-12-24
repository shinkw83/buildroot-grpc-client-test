#!/bin/bash

# This script sets up the environment for cross-compiling for the RK3506 platform.
# It configures the necessary toolchain and environment variables.

cmake -Bbuild -H. -DCMAKE_TOOLCHAIN_FILE=/home/skw/project/lyra-ultra/fox/buildroot/output/latest/host/share/buildroot/toolchainfile.cmake -DCMAKE_FIND_ROOT_PATH=/home/skw/project/lyra-ultra/fox/buildroot/output/latest/host/arm-buildroot-linux-gnueabihf/sysroot

cmake --build build/