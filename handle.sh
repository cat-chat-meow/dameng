#!/bin/bash

docker cp /opt/dm8_20230104_x86_rh6_64.iso demo_deps:/opt/

# docker install
su - dmdba
cd /mnt/
./DMInstall.bin -i
# choose
# en, no key, yes timezone 21, 1 typical, dir /dm8