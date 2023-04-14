#!/bin/bash

. ./check_usr.sh

check_usr dmdba

echo "db config"
cd /dm8/bin
./dminit help
./dminit path=/dm8/data
./dminit path=/dm8/data PAGE_SIZE=32 EXTENT_SIZE=32 CASE_SENSITIVE=y CHARSET=1 DB_NAME=DMDB INSTANCE_NAME=DBSERVER PORT_NUM=5237
