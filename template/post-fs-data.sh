#!/system/bin/sh
MODDIR=${0%/*}
BIN="$MODDIR/bin/pid_wrap"
if [ -x "$BIN" ]; then
    "$BIN"
fi
