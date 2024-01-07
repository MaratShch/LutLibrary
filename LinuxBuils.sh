#!/bin/bash

WORK_DIR=$PWD

CMAKE_BIN=~/cmake-3.28.0-rc2-linux-x86_64/bin/cmake
BUILD_FOLDER=$WORK_DIR/build
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

echo 'Start make generation...'
$CMAKE_BIN -DCMAKE_INSTALL_PREFIX=$INSTALL_FOLDER -DLUT_LIB_TESTS=ON ..

echo 'Start build...'
make -j 4 && make install -j 4
echo 'Build completed'

