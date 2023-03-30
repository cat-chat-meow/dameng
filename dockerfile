FROM debian:bullseye AS dameng_cpp_demo_basin

COPY sources.list /etc/apt/
COPY vimrc /root/.vimrc

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

FROM dameng_cpp_demo_basin AS dameng_cpp_demo_runtime

# check odbc install
RUN odbcinst -j

COPY etc/odbc.ini /etc/odbc.ini
COPY etc/odbcinst.ini /etc/odbcinst.ini

WORKDIR /app
COPY . /app

# RUN g++ -o main main.cpp

# CMD ["./main"]
