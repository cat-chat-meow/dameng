#!/bin/bash
BASE_DIR=$(cd "$(dirname "$0")"; pwd)

file_name=dm8_20230104_x86_rh6_64

[ ! -e $BASE_DIR/${file_name}.zip ] && \
    echo "File is not exsit." && exit 0

echo "unzip ${file_name}.zip"
unzip ${file_name}.zip
