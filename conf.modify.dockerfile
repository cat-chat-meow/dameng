# base 达梦8 官方 docker 镜像 支持 odbc 和 dpi

FROM dm8_single:v8.1.2.128_ent_x86_64_ctm_pack4 AS modfiy_dm8

# cat /etc/issue
# Ubuntu 16.04.4 LTS \n \l

COPY sources.list.ubuntu /etc/apt/sources.list

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    vim \
    gcc \
    g++ \
    gdb \
    nano

RUN apt-get install -y \
    unixodbc unixodbc-dev


# ---------------------------------
# mysql test
RUN DEBIAN_FRONTEND=noninteractive apt-get install -y \
    mysql-server
RUN apt-get install -y \
    libboost-all-dev \
    zlib1g-dev \
    libmysqlclient-dev

# ---------------------------------

COPY vimrc /root/.vimrc

FROM modfiy_dm8 AS dm_cpp_demo_runtime

# check odbc install
RUN odbcinst -j

COPY etc/odbc.ini /etc/odbc.ini
COPY etc/odbcinst.ini /etc/odbcinst.ini

WORKDIR /app

COPY dpi_test /app/dpi_test
COPY etc /app/etc
COPY include /app/include
COPY odbc_test /app/odbc_test

# RUN g++ -o main main.cpp

# CMD ["./main"]
