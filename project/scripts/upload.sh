#!/usr/bin/env bash

# config

source $(dirname $0)/config.sh

# entry

task:start Uploading executable
scp ${BUILD_DIR}/final-phase ${SSH_HOST}:${SSH_PATH}/ || task:fail
task:end
