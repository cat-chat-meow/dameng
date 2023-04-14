#!/bin/bash

. ./check_usr.sh

check_usr dmdba

echo "db install"
cd /mnt/ && ./DMInstall.bin -i
# choose
# en, no key, yes timezone 21, 1 typical, dir /dm8