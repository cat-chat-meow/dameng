#!/bin/bash

if [ "$1" == "demo0" ]; then

    echo "------------------start mysql ..."
    sudo /etc/init.d/mysql start

fi 

exec "$@"

make clean -C cppdbi && make -C cppdbi package BITS=64
