#!/usr/bin/env bash

# config

ROOT_DIR=$(dirname $(dirname $(realpath $0)))
BUILD_DIR=${ROOT_DIR}/build
SRC_DIR=${ROOT_DIR}/src

SSH_HOST=ls2k-local
SSH_PATH=/root/bin
MAKE_JOBS=$(nproc)

# utils

err() {
  echo -e "\033[31m$@\033[0m"
  exit 1
}

info() {
  echo -e "\033[32m$@\033[0m"
}

task:start() {
  TASK_TOPIC="$@"
  TIME_START=$(date +%s%N)
  info ">>> $TASK_TOPIC"
}

task:end() {
  TIME_END=$(date +%s%N)
  TIME_DIFF=$((TIME_END - TIME_START))
  info "<<< $TASK_TOPIC [$((TIME_DIFF / 1000000)) ms]"
}

task:fail() {
  err "!!! $TASK_TOPIC"
}
