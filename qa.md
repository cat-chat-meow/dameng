# QA

- demo3 privileged 调试 达梦
- deps docker no privileged 使用 mysql 作为参考

## step

```bash
# host
docker cp dm8_20230104_x86_rh6_64.zip demo_deps:/opt/

# docker mount iso
cd /opt/
unzip dm8_20230104_x86_rh6_64.zip
mount -o loop /opt/dm8_20230104_x86_rh6_64.iso /mnt

# db install
su - dmdba
cd /mnt/ && ./DMInstall.bin -i
# choose
# en, no key, yes timezone 21, 1 typical, dir /dm8

# db config
su - root
mkdir -p /dm8/data && chown dmdba:dinstall -R /dm8/data && chmod -R 755 /dm8/data 
su - dmdba
cd /dm8/bin
./dminit help
./dminit path=/dm8/data
./dminit path=/dm8/data PAGE_SIZE=32 EXTENT_SIZE=32 CASE_SENSITIVE=y CHARSET=1 DB_NAME=DMDB INSTANCE_NAME=DBSERVER PORT_NUM=5237

# registration service
su - root
cd /dm8/script/root
./dm_service_installer.sh -t dmserver -dm_ini /dm8/data/DAMENG/dm.ini -p DMSERVER
```

## docker 内使用`systemctl`命令报错

使用`/usr/sbin/init`同时添加`--privileged=true`参数就能够使用`systemctl`了

```bash
docker run -it --name name <image_name>
# 改为
docker run -it --name name --privileged=true <image_name> /sbin/init
```

## 关于`privileged`选项

mount -o loop dm8_20230104_x86_rh6_64.iso /mnt
mount: /mnt: mount failed: Unknown error -1

docker 启动没有使用 privileged=true

但是如果加上 privileged=true mysql 就启动不了了 真是绝活

对此问题 **chatGPT** 的解释为

> 在 Docker 中使用 privileged 模式会暴露出更多的系统资源，包括主机的设备、内核和网络等。这可能会对系统的安全性产生影响，因此最好避免在生产环境中使用 privileged 模式。  
> 对于无法正常启动 MySQL 的问题，可能是因为在 privileged 模式下，Docker 默认会挂载主机的 /etc 目录。您可以尝试在启动命令中指定 MySQL 配置文件的目录，如：
>
> ```bash
> docker run --privileged=true -v /path/to/mysql/conf:/etc/mysql/conf.d -d mysql
> ```
>
> 对于无法正常使用 mount 命令的问题，可能是因为在非 privileged 模式下，Docker 需要明确指定容器与主机之间的文件系统类型。您可以尝试在启动命令中指定文件系统类型，如：
>
> ```bash
> docker run -v /host/path:/container/path:rw,type=bind alpine
> ```
>
> 其中，type=bind 指定了文件系统类型为 bind mount。

但是并不好用

## 调试通过的示例

- [odbc_conn](./odbctest/odbc_conn.c)
- [odbc_dml](./odbctest/odbc_dml.c)
- [odbc_bind](./odbctest/odbc_bind.c) 之前存在内存偏移，是因为类型不匹配导致 已修复

[odbc_lob](./odbctest/odbc_lob.c) 未调试

## 方案记录

由于项目代码使用 Debian:jessie 编译，无法直接在 base 官方镜像的 docker 中正常运行。

故而采用如下几种方案
1. 使用 base 项目代码的镜像，在镜像里手动安装配置达梦数据库，