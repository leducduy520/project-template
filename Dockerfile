FROM ubuntu:latest

RUN  apt-get update
RUN  apt upgrade -y
RUN  apt-get install -y build-essential \
cmake git doxygen python3 python3-pip \
ninja-build graphviz pkg-config dos2unix cmake-format \
libxrandr-dev \
libxcursor-dev \
libudev-dev \
libopenal-dev \
libflac-dev \
libvorbis-dev \
libgl1-mesa-dev \
libegl1-mesa-dev \
libdrm-dev \
libgbm-dev \
libfreetype6-dev \
libssl-dev \
clang-tidy clang-format
