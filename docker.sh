#!/bin/bash

if [ -z $1 ]; then
    echo "no option, exit" && echo " "
    echo "Usage: $0 [OPTION]"
    echo "OPTION: run [r] | build [b]"
    exit 1
fi


docker_name=dm_demo
run_name=dm_demo

. ./docker_kill.sh
rm_docker $run_name


case $1 in
run)
    docker run -it --name ${run_name} ${docker_name}:latest
    exit 0
    ;;
r)
    docker run -it --name ${run_name} ${docker_name}:latest
    exit 0
    ;;
build)
    echo "build ${docker_name}"
    echo " "
    docker build -t ${docker_name} .
    exit 0
    ;;
b)
    echo "build ${docker_name}"
    echo " "
    docker build -t ${docker_name} .
    exit 0
    ;;
*)
    ;;
esac
