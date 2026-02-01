# Use specific Ubuntu version for reproducible builds
FROM ubuntu:22.04

# Set environment variables to avoid interactive prompts
ENV DEBIAN_FRONTEND=noninteractive \
    TZ=UTC

# Install CMake 3.30 from Kitware and all other packages in a single layer
# This reduces image size and number of layers
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        ca-certificates \
        gnupg \
        wget && \
    wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | gpg --dearmor - | tee /usr/share/keyrings/kitware-archive-keyring.gpg >/dev/null && \
    echo 'deb [signed-by=/usr/share/keyrings/kitware-archive-keyring.gpg] https://apt.kitware.com/ubuntu/ jammy main' | tee /etc/apt/sources.list.d/kitware.list >/dev/null && \
    apt-get update && \
    apt-get install -y --no-install-recommends \
        autoconf \
        automake \
        build-essential \
        clang-format \
        clang-tidy \
        cmake=3.30.* \
        cmake-data=3.30.* \
        cmake-format \
        curl \
        doxygen \
        dos2unix \
        git \
        graphviz \
        lcov \
        libdrm-dev \
        libegl1-mesa-dev \
        libflac-dev \
        libfreetype6-dev \
        libgbm-dev \
        libgl1-mesa-dev \
        libopenal-dev \
        libssl-dev \
        libtool \
        libudev-dev \
        libvorbis-dev \
        libx11-dev \
        libxcursor-dev \
        libxi-dev \
        libxrandr-dev \
        ninja-build \
        pkg-config \
        python3 \
        python3-pip \
        tar \
        unzip \
        zip &&\
    apt-get clean && \
    rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

# Create a non-root user for security
RUN groupadd -r builder && \
    useradd -r -g builder -m -s /bin/bash builder

# Switch to non-root user
USER builder

# Set working directory
WORKDIR /home/builder

CMD ["/bin/bash"]