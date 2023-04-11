# 仅安装 达梦数据库 没做其他测试，为获取达梦头文件

FROM debian:bullseye

COPY sources.list /etc/apt/

RUN apt-get update
RUN apt-get install -y \
    wget \
    sudo \
    vim \
    unzip

COPY vimrc /root/.vimrc

# add group
RUN groupadd dinstall

# add user
RUN useradd -g dinstall -m -d /home/dmdba -s /bin/bash dmdba && \
    echo dmdba:123 | chpasswd 

# 修改文件打开最大数
RUN sh -c 'echo "dmdba hard nofile 65536" >> /etc/security/limits.conf' && \
    sh -c 'echo "dmdba soft nofile 65536" >> /etc/security/limits.conf' && \
    sh -c 'echo "dmdba hard stack 32768" >> /etc/security/limits.conf'  && \
    sh -c 'echo "dmdba soft stack 16384" >> /etc/security/limits.conf'

WORKDIR /app
COPY . /app

RUN wget https://download.dameng.com/eco/adapter/DM8/202302/dm8_20230104_x86_rh6_64.zip
# RUN mount -o loop /app/dm8_setup_rh7_64_ent_8.1.1.45_20191121.iso /mnt

RUN mkdir /dm8
# 修改权限
RUN chown dmdba:dinstall -R /dm8/ && chmod -R 755 /dm8

# USER dmdba
