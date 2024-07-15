FROM ubuntu:latest

RUN  apt-get update
RUN  apt-get install -y build-essential
RUN  apt-get install -y cmake
RUN  apt-get install -y git doxygen
RUN  apt-get install -y python3 python3-pip
RUN  apt-get install -y ninja-build
RUN  apt-get install -y graphviz pkg-config
RUN  apt-get install -y dos2unix
RUN  apt-get install -y cmake-format
RUN apt install -y \
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
libfreetype6-dev
RUN apt install -y clang-tidy clang-format
