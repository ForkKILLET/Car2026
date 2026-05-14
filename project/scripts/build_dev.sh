#!/usr/bin/env bash

# config

source $(dirname $0)/config.sh

# entry

task:start Entering dev build directory
cd ${DEV_BUILD_DIR} || task:fail
task:end

task:start Running cmake
cmake ${ROOT_DIR} -DTARGET=dev || task:fail
task:end

task:start Running make
make -j${MAKE_JOBS} || task:fail
task:end
