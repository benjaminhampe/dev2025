#!/bin/bash
PWD="$(pwd)"
BUILD_DIR=$PWD/build/win64_RelWithDebInfo_shared
MAX_THREADS=$(nproc)

# Check if an argument was provided
if [ -z "$1" ]; then
  JOBS=$MAX_THREADS  # No argument: use number of processors
else
  JOBS=$1  # Use the provided argument
fi

echo "PWD = $PWD"
echo "BUILD_DIR = $BUILD_DIR"
echo "MAX_THREADS = $MAX_THREADS"
echo "JOBS = $JOBS"

mkdir -p $BUILD_DIR
cd $BUILD_DIR

cmake -G Ninja \
    -DCMAKE_BUILD_TYPE=RelWithDebInfo \
    ../../

# Run ninja with the chosen job count
ninja -j"$JOBS"

# mingw32-make -j$1

    # -DCMAKE_C_COMPILER=clang.exe \
    # -DCMAKE_CXX_COMPILER=clang++.exe \
    # -DCMAKE_LINKER=lld.exe \
    # -DCMAKE_AR=llvm-ar \
    # -DCMAKE_RANLIB=llvm-ranlib \
    # -DCMAKE_RC_COMPILER=llvm-rc.exe \
    # -DCMAKE_SYSTEM_NAME=Windows \
    # -DCMAKE_C_FLAGS="-target x86_64-w64-windows-gnu"
    # -DCMAKE_CXX_FLAGS="-target x86_64-w64-windows-gnu"
    # -DCMAKE_EXE_LINKER_FLAGS="-fuse-ld=lld" \
    #
#
#   -DCMAKE_PREFIX_PATH=$QT_DIR/lib/cmake \
#   -DCMAKE_MODULE_PATH=$QT_DIR/lib/cmake \
#
