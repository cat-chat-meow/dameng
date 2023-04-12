#!/bin/bash

docker cp dm8_20230104_x86_rh6_64.zip demo_deps:/opt/

# docker install
cd /opt/
unzip dm8_20230104_x86_rh6_64.zip
mount -o loop /opt/dm8_20230104_x86_rh6_64.iso /mnt

su - dmdba
cd /mnt/
./DMInstall.bin -i
# choose
# en, no key, yes timezone 21, 1 typical, dir /dm8