#!/bin/bash

check_usr(){
    if [ $# -lt 1 ]; then
        echo "don't know user"
        exit 1
    fi

    if [ "$USER" != "$1" ]; then
        echo "$0 must be run as $1!"
        exit 1
    fi

    echo "check usr [$1] success!"
}
