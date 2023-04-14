# 这个 docker 因为使用 deps base Debian jessie
# 无法更新安装任何包 很难使用

FROM vdna-deps-env:latest

# COPY sources.list.debian8 /etc/apt/

RUN apt-get update
RUN apt-get install -y --force-yes \
    unzip \
    vim \
    wget

# ---------------------------------
# mysql test
RUN sed -i "s/127.0.0.1/0.0.0.0/g" /etc/mysql/my.cnf
# ---------------------------------

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

RUN wget -q https://download.dameng.com/eco/adapter/DM8/202302/dm8_20230104_x86_rh6_64.zip && \
    ls -ld dm8_20230104_x86_rh6_64.zip

RUN mkdir -p /dm8 /dm8/data
# 修改权限
RUN chown dmdba:dinstall -R /dm8/ && chmod -R 755 /dm8 && \
    chown dmdba:dinstall -R /dm8/data && chmod -R 755 /dm8/data

RUN bash mount_iso.sh

RUN sed -i "s/PWD=SYSDBA001/PWD=SYSDBA/g" etc/config.ini
