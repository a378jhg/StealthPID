#!/system/bin/sh
MODDIR=${0%/*}
LOG="/data/local/tmp/superhide.log"
BIN="$MODDIR/bin/pid_wrap"
until [ "$(getprop sys.boot_completed)" = "1" ]; do sleep 2; done
sleep 5
if [ ! -x "$BIN" ]; then exit 1; fi
"$BIN"
echo "$(date): 开机压底完成" >> $LOG
while true; do
    CURRENT_PID=$(cat /proc/sys/kernel/ns_last_pid 2>/dev/null)
    case "$CURRENT_PID" in ''|*[!0-9]*) CURRENT_PID=0 ;; esac
    if [ "$CURRENT_PID" -gt 3000 ]; then
        "$BIN"
        sleep 2
    fi
    sleep 1
done
