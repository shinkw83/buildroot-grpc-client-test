#!/bin/bash

# This script sets up the environment for cross-compiling for the RK3506 platform.
# It configures the necessary toolchain and environment variables.

cmake -Bbuild -H. -DCMAKE_PREFIX_PATH=$HOME/.local

cmake --build build/