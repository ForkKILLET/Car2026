#!/bin/sh

ACTION="$1"

case "$ACTION" in
  bound|renew)
    if pidof ssh >/dev/null; then
      exit 0
    fi
    /etc/init.d/S90-ssh-tunnel start
    ;;
  deconfig)
    /etc/init.d/S90-ssh-tunnel stop
    ;;
esac
