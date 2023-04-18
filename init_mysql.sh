#!/bin/bash

/etc/init.d/mysql start 

started=false
for i in {1..10}; do
    mysqladmin ping > /dev/null 2>&1
    if [[ $? -eq 0 ]]; then
        started=true
        break
    fi
    sleep 1
done
if [ "$started" != true ]; then
    echo >&2 "ERROR: MySQL failed to start"
    exit 1
fi

mysqladmin password $MYSQL_ROOT_PASSWORD
