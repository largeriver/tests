#!/bin/bash
MAKE_ROOT_DIR=`pwd`
MKFILE_TMPLATE_DIR=$MAKE_ROOT_DIR/buildenv
CC=arm-linux-androideabi-gcc

export MAKE_ROOT_DIR
export MKFILE_TMPLATE_DIR
export CC