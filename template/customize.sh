#!/system/bin/sh
SKIPUNZIP=0
ui_print "- 正在安装 StealthPID 隐匿模块"
set_perm_recursive $MODPATH 0 0 0755 0644
set_perm $MODPATH/bin/pid_wrap 0 0 0755
set_perm $MODPATH/service.sh 0 0 0755
set_perm $MODPATH/post-fs-data.sh 0 0 0755
