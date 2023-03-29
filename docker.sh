#!/bin/bash

PARA_NUM=$#

# tag 1: -b or -r
TAG_1=$1
# tag 2: -name
TAG_2=$3

if [[ $PARA_NUM -ne 2 && $PARA_NUM -ne 4 ]];then
    echo "Insufficient parameters, exit" && echo " "
    echo "Usage: $0 [-b DOCKER NAME]"
    echo "       $0 [-r DOCKER NAME] [-name RUN NAME]"
    exit 1
fi

CUSTOM_BRIDGE_NAME=dm_network

docker_name=dm_demo
run_name=dm_demo

docker_name=$2
run_name=$4

. ./docker_kill.sh


check_para(){
    [[ $PARA_NUM -ne $1 ]] && echo "check para err, para $1" && exit 1
    echo "check para num passed"

    case $1 in
    4)
        [[ $TAG_1 != "-r" ]] && echo "tag -r err" && exit 1
        [[ $TAG_2 != "-name" ]] && echo "tag -name err" && exit 1
        ;;
    2)
        [[ $TAG_1 != "-b" ]] && echo "tag -b err" && exit 1
        ;;
    *)
        ;;
    esac
}


docker_build() {
    if [ -z $1 ]; then
        echo "no option, exit"
        exit 1
    fi

    echo "build ${1}"
    echo " "    

    case $1 in
    demo0)
        docker build -t ${1} .
        ;;
    demo1)
        docker build -t ${1} -f conf.modify.dockerfile .
        ;;
    *);;
    esac
}

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
    demo0)
        docker run -it \
            --name ${run_name} \
            --network ${CUSTOM_BRIDGE_NAME} \
            --ip 172.20.0.6 \
            ${docker_name}:latest
        ;;
    demo1)
        docker run -it \
            --name ${run_name} \
            -e LD_LIBRARY_PATH=/opt/dmdbms/bin \
            -e DM_HOME=/opt/dmdbms \
            ${docker_name}:latest
        ;;
    *)
        ;;
    esac
}


case $1 in
-r)
    check_para 4
    rm_docker $run_name
    docker_run $2
    exit 0
    ;;
-b)
    check_para 2
    docker_build $2
    exit 0
    ;;
*)
    ;;
esac
