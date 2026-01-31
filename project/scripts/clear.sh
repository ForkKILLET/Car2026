#!/usr/bin/env bash

# config

source $(dirname $0)/config.sh

# entry

task:start Clearing out directory
find ${BUILD_DIR} -mindepth 1 -exec rm -rf {} + || task:fail
task:end
