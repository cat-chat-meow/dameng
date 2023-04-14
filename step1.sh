#!/bin/bash

. ./check_usr.sh

check_usr root

echo "mount iso"
mount -o loop /app/dm8_20230104_x86_rh6_64.iso /mnt
