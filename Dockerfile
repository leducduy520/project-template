
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
ADD https://github.com/mongodb/mongo-cxx-driver.git#r3.10.2 /usr/src/mongo-cxx-driver
RUN cmake -S /usr/src/mongo-cxx-driver -B /usr/src/mongo-cxx-driver/build --fresh -DCMAKE_INSTALL_PREFIX="/usr/src/mongo-cxx-driver/install" -DBUILD_SHARED_LIBS=OFF -DENABLE_BSONCXX_POLY_USE_IMPLS=ON -DBUILD_VERSION="3.10.2" -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_CXX_FLAGS_INIT="-fPIC" 
RUN cmake --build /usr/src/mongo-cxx-driver/build -t install --config RelWithDebInfo -j8
