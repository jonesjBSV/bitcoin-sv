# Use Ubuntu 22.04 as base image
FROM ubuntu:22.04

# Prevent interactive prompts during package installation
ENV DEBIAN_FRONTEND=noninteractive

# Install basic build requirements and GCC 10
RUN apt-get update && apt-get install -y \
    build-essential \
    gcc-10 \
    g++-10 \
    libtool \
    autotools-dev \
    automake \
    python3 \
    pkg-config \
    libssl-dev \
    libevent-dev \
    bsdmainutils \
    git \
    # Berkeley DB
    libdb-dev \
    libdb++-dev \
    # Boost libraries
    libboost-system-dev \
    libboost-filesystem-dev \
    libboost-chrono-dev \
    libboost-program-options-dev \
    libboost-test-dev \
    libboost-thread-dev \
    # Optional dependencies
    libminiupnpc-dev \
    libzmq3-dev \
    libgoogle-perftools-dev \
    # Test dependencies
    python3-ecdsa \
    python3-zmq \
    python3-bip32utils \
    # IPv6 multicast dependencies
    iproute2 \
    iputils-ping \
    && rm -rf /var/lib/apt/lists/*

# Set GCC 10 as the default compiler
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-10 100 \
    && update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-10 100

# Set working directory
WORKDIR /bitcoin-sv

# Copy source code (assuming it's in the build context)
COPY . .

# Build Bitcoin SV with IPv6 multicast support
RUN ./autogen.sh && \
    ./configure \
        --enable-hardening \
        --enable-ipv6 \
        --enable-ipv6-multicast \
        CPPFLAGS="-I$(pwd)/src \
                  -I$(pwd)/src/config \
                  -I$(pwd)/src/net \
                  -I$(pwd)/src/consensus \
                  -I$(pwd)/src/primitives \
                  -I$(pwd)/src/script \
                  -I$(pwd)/src/univalue/include \
                  -I$(pwd)/src/crypto" && \
    make -j1 && \
    make install

# Verify binary exists and is executable
RUN test -x /usr/local/bin/bitcoind || exit 1

# Default command with IPv6 options
CMD ["bitcoind", "-listen", "-discover", "-ipv6", "-multicast"]