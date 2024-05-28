# GCC support can be specified at major, minor, or micro version
# (e.g. 8, 8.2 or 8.2.0).
# See https://hub.docker.com/r/library/gcc/ for all supported GCC
# tags from Docker Hub.
# See https://docs.docker.com/samples/library/gcc/ for more on how to use this image
FROM gcc:latest

RUN apt-get update
RUN apt-get install -y build-essential
RUN apt-get install -y cmake
RUN apt-get install -y git doxygen
RUN apt-get install -y python3 python3-pip
RUN apt-get install -y ninja-build
RUN apt-get install -y graphviz pkg-config
RUN apt-get install -y dos2unix
RUN apt-get install -y cmake-format

VOLUME [ "/data" ]
# These commands copy your files into the specified directory in the image
# and set that as the working location
COPY . /usr/src/myapp
WORKDIR /usr/src/myapp

# This command compiles your app using GCC, adjust for your source code
RUN make config-ubuntu
RUN cmake --build --preset=ubuntu-complete

# This command runs your application, comment out this line to compile only
CMD ["./out/install/ubuntu/bin/app"]

LABEL Name=projecttemplate Version=0.0.1
