#!/bin/bash

WORK_DIR=$PWD

# Default to high performance
ACCURACY_MODE="fast"
CMAKE_ACCURACY_FLAG="-DENABLE_HIGH_ACCURACY=OFF" # Default CMake flag

# Parse command line arguments
if [ "$1" == "--accuracy" ]; then
  if [ "$2" == "high" ]; then
    ACCURACY_MODE="high"
    CMAKE_ACCURACY_FLAG="-DENABLE_HIGH_ACCURACY=ON"
    echo "Setting accuracy mode to: HIGH"
  elif [ "$2" == "fast" ]; then
    ACCURACY_MODE="fast"
    CMAKE_ACCURACY_FLAG="-DENABLE_HIGH_ACCURACY=OFF"
    echo "Setting accuracy mode to: FAST"
  else
    echo "Invalid accuracy mode: $2. Use 'high' or 'fast'."
    echo "Defaulting to FAST performance."
  fi
fi

CMAKE_BIN=cmake #~/cmake-3.30.2-linux-x86_64/bin/cmake
BUILD_FOLDER=$WORK_DIR/build_${ACCURACY_MODE}
INSTALL_FOLDER=$WORK_DIR/install

if [ -d $BUILD_FOLDER ]; then
  rm -fr $BUILD_FOLDER
fi
if [ -d $INSTALL_FOLDER ]; then
  rm -fr $INSTALL_FOLDER
fi
sync

mkdir -p $BUILD_FOLDER
mkdir -p $INSTALL_FOLDER
sync
cd $BUILD_FOLDER


echo 'Start generate build configuration ...'
$CMAKE_BIN -DCMAKE_INSTALL_PREFIX=$INSTALL_FOLDER -DLUT_LIB_TESTS=ON "$CMAKE_ACCURACY_FLAG" ..

if [ $? -ne 0 ]; then
  echo "CMake configuration failed."
  exit 1
fi

echo 'Start build and install ...'
$CMAKE_BIN --build . --target install -j $(nproc) --config Release --verbose

# Check if build and install failed
if [ $? -ne 0 ]; then
  echo "Build or Install failed."
  exit 1
fi

echo 'Build and Install completed'
