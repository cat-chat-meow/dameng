#!/bin/bash

if [ "$1" == "demo0" ]; then

    echo "------------------start mysql ..."
    sudo /etc/init.d/mysql start

fi 

exec "$@"
