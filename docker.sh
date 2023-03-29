#!/bin/bash

if [ -z $1 ]; then
    echo "no option, exit" && echo " "
    echo "Usage: $0 [OPTION]"
    echo "OPTION: run [r] | build [b]"
    exit 1
fi

CUSTOM_BRIDGE_NAME=dm_network

DOCKER_NAME=dm_demo
RUN_NAME=dm_demo

. ./docker_kill.sh
rm_docker $RUN_NAME


docker_run() {
    if [ -z $1 ]; then
        echo "no option, exit"
        exit 1
    fi

    has_bridge=$(docker network ls | grep ${CUSTOM_BRIDGE_NAME})

    # create bridge
    [ -z "$has_bridge" ] && echo -e "create bridge\n" && \
        docker network create --driver bridge ${CUSTOM_BRIDGE_NAME} \
        --subnet=172.20.0.0/24

    case $1 in
    dmdb)
        rm_docker dm8_01
        docker run -d \
            -p 5236:5236 \
            --restart=always \
            --name dm8_01 \
            --network ${CUSTOM_BRIDGE_NAME} \
            --ip 172.20.0.100 \
            --privileged=true \
            -e PAGE_SIZE=16 \
            -e LD_LIBRARY_PATH=/opt/dmdbms/bin \
            -e INSTANCE_NAME=dm8_01 \
            -v /data/dm8_01:/opt/dmdbms/data \
            dm8_single:v8.1.2.128_ent_x86_64_ctm_pack4
        ;;
    demo)
        docker run -it \
            --name ${RUN_NAME} \
            --network ${CUSTOM_BRIDGE_NAME} \
            --ip 172.20.0.6 \
            -v /opt/dmdbms/bin:/opt/dmdbms/bin \
            ${DOCKER_NAME}:latest
        ;;
    *)
        ;;
    esac
}



case $1 in
run)
    docker_run $2
    exit 0
    ;;
r)
    docker_run $2
    exit 0
    ;;
build)
    echo "build ${DOCKER_NAME}"
    echo " "
    docker build -t ${DOCKER_NAME} .
    exit 0
    ;;
b)
    echo "build ${DOCKER_NAME}"
    echo " "
    docker build -t ${DOCKER_NAME} .
    exit 0
    ;;
*)
    ;;
esac
