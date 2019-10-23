FROM ubuntu:18.04 AS base

ENV LD_LIBRARY_PATH /usr/local/lib

RUN apt-get update \
    && apt-get install -y \
    build-essential \
    cmake=3.10.2-1ubuntu2.18.04.1 \
    git \
    g++ \
    libevent-dev=2.1.8-stable-4build1 \
    libpthread-stubs0-dev=0.3-4 \
    libsecp256k1-dev=0.1~20170810-1 \
    libssl-dev=1.1.1-1ubuntu2.1~18.04.4 \
    python3


# FROM base AS build

RUN apt-get install -y \
    autoconf \
    automake \
    clang-5.0=1:5.0.1-4 \
    libc++-dev=6.0-2 \
    libtool \
    libxml2-dev \
    libxslt-dev \
    pkg-config \
    python-dev \
    wget

WORKDIR /src
## build boost 1.65.0
RUN wget http://dl.bintray.com/boostorg/release/1.65.0/source/boost_1_65_0.tar.gz \
    && tar xzvf boost_1_65_0.tar.gz \
    && cd boost_1_65_0 \
    && ./bootstrap.sh --prefix=/usr/ \
    && ./b2 --with-system \
    && ./b2 install

WORKDIR /src
## build gflags
RUN wget https://github.com/gflags/gflags/archive/v2.2.2.tar.gz \
    && tar xzf v2.2.2.tar.gz \
    && cd gflags-2.2.2 && mkdir build && cd build \
    && cmake .. -DBUILD_SHARED_LIBS=ON -DBUILD_STATIC_LIBS=ON -DBUILD_gflags_LIB=ON \
    && make -j \
    && make install

WORKDIR /src
## build gtest
RUN git clone -b release-1.8.1 https://github.com/google/googletest \
    && cd googletest && mkdir build && cd build \
    && cmake -DCMAKE_BUILD_TYPE=Release .. \
    && make -j \
    && make install

WORKDIR /src
## build glog
RUN git clone https://github.com/google/glog.git \
    && cd glog \
    && cmake -DCMAKE_BUILD_TYPE=Release -H. -Bbuild -G "Unix Makefiles" \
    && cmake --build build -- -j \
    && cmake --build build --target install

WORKDIR /src
RUN git clone -b 1.22 https://github.com/google/leveldb \
    && cd leveldb \
    && mkdir -p build && cd build \
    && cmake -DCMAKE_BUILD_TYPE=Release .. && cmake --build . -- -j \
    && make install

WORKDIR /src
RUN git clone -b v6.3.6 https://github.com/facebook/rocksdb \
    && cd rocksdb \
    && make shared_lib && make install

# FROM base AS release

ENV RUN_USER medium
ENV RUN_GROUP medium
ENV USER_PASSWORD medium@1234
ENV DEBIAN_FRONTEND noninteractive
RUN useradd -ms /bin/bash ${RUN_USER}

RUN apt-get -y install \
    gdb=8.1-0ubuntu3 \
    gdbserver=8.1-0ubuntu3 \
    openssh-server \
    rsync=3.1.2-2.1ubuntu1

# COPY --from=build /usr/local/ /usr/local/
# COPY --from=build /usr/include/ /usr/include/

USER ${RUN_USER}
COPY entrypoint.sh /
USER root
CMD ["/entrypoint.sh", "-fg"]
ENTRYPOINT ["/bin/bash", "--"]

EXPOSE 22
