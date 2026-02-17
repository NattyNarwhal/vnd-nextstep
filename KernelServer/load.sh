#!/bin/sh
cp vnddrv_reloc /dev2
old_pwd=`pwd`
cd /dev2
kl_util -a vnddrv_reloc
sh -x $old_pwd/makedev vnd0
$old_pwd/vnconfig /dev2/vnd0c /me/CultureShock.img
