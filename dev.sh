#!/bin/bash

# 安装好 demo3 使用 commit 导出
# env_dm8_base_deps base `docker commit demo_deps env_dm8_base_deps`

run_name=dev_co_dm8
docker_name=env_dm8_base_deps
echo "fuck"
path_dm=/dm8
docker run -it \
    --privileged=true \
    --name ${run_name} \
    -e LD_LIBRARY_PATH=$path_dm/bin \
    -e DM_HOME=$path_dm \
    ${docker_name}:latest /sbin/init

