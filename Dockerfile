
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
RUN  apt-get install -y wget
ADD https://github.com/mongodb/mongo-cxx-driver.git#r3.10.2 /usr/src/mongo-cxx-driver
RUN cmake -S /usr/src/mongo-cxx-driver -B /usr/src/mongo-cxx-driver/build --fresh -DCMAKE_INSTALL_PREFIX="/usr/src/external/mongo-cxx-driver" -DBUILD_SHARED_LIBS=OFF -DENABLE_BSONCXX_POLY_USE_IMPLS=ON -DBUILD_VERSION="3.10.2" -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_CXX_FLAGS_INIT="-fPIC" 
RUN cmake --build /usr/src/mongo-cxx-driver/build -t install --config RelWithDebInfo -j8
ADD mongosh-2.2.15-linux-x64.tgz /usr/src
ADD container_build.sh /usr/src
RUN apt-get install -y libx11-dev libxi-dev libxrandr-dev libgl1-mesa-dev libxrandr-dev libudev-dev
ADD https://github.com/SFML/SFML.git /usr/src/SFML
RUN cmake -S /usr/src/SFML -B /usr/src/SFML/build --fresh -DCMAKE_INSTALL_PREFIX="/usr/src/external/SFML" -DBUILD_SHARED_LIBS=OFF -DCMAKE_CXX_STANDARD=17 -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_CXX_FLAGS_INIT="-fPIC"
RUN cmake --build /usr/src/SFML/build -t install --config RelWithDebInfo -j8
