#!/bin/bash

git submodule update --init --recursive
pushd javacpp && mvn package && popd
pushd ddc/third_party/libcuckoo/cityhash-1.1.1 && autoreconf -fis && CXX=g++-6 ./configure && make -j 10 && popd
pushd ddc && ./bootstrap.sh && make && popd
sed -i "s#REPLACEME/remotebuf#$(pwd)#g" ucb/remotebuf/config/RemoteBuf.java
CXXFLAGS=-DRDMA make java
cp ddc/src/server/bladeallocmain .
