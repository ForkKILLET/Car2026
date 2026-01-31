#!/bin/sh

LOG=/var/log/ssh-tunnel.log

log() {
  echo "[$(date '+%Y-%m-%d %H:%M:%S')] $@" >> $LOG
}

start() {
  log start
  ssh -N -T \
    -R 0.0.0.0:10023:localhost:22 \
    -p 10022 \
    -i /root/.ssh/tunnel_ed25519 \
    tunnel@genshin.asm.ms \
    >> /var/log/ssh.log 2>&1 &
}

stop() {
  log stop
  killall ssh 2>/dev/null
}

ACTION="${1:-start}"

case "$ACTION" in
  start)
    start
    ;;
  stop)
    stop
    ;;
  restart)
    stop
    start
    ;;
esac
