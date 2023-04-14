#!/bin/bash

. ./check_usr.sh

check_usr root

dir=/dm8/data

echo "mkdir $dir"
mkdir -p $dir
chown dmdba:dinstall -R $dir && chmod -R 755 $dir
