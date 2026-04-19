#!/bin/bash

WORK_DIR=$PWD
BUILD_FOLDER1=$WORK_DIR/build_fast
BUILD_FOLDER2=$WORK_DIR/build_high
INSTALL_FOLDER=$WORK_DIR/install

sync

if [ -d $BUILD_FOLDER1 ]; then
  rm -fr $BUILD_FOLDER1
fi
if [ -d $BUILD_FOLDER2 ]; then
  rm -fr $BUILD_FOLDER2
fi
if [ -d $INSTALL_FOLDER ]; then
  rm -fr $INSTALL_FOLDER
fi

sync
