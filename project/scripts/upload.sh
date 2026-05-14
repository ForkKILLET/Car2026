#!/usr/bin/env bash

# config

source $(dirname $0)/config.sh

# entry

MAIN_EXECUTABLE=main
EXECUTABLE=${1:-${MAIN_EXECUTABLE}}
UPLOAD_URL=${SSH_HOST}:${SSH_PATH}

task:start Uploading executable to ${UPLOAD_URL}
scp ${BUILD_DIR}/${EXECUTABLE} ${UPLOAD_URL} || task:fail
task:end
