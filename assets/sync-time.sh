#!/usr/bin/env sh

date -s "$(curl -sI http://www.baidu.com | grep -i '^date:' | cut -d' ' -f2-)"
