#!/bin/bash

git submodule update --init
pushd javacpp && mvn package && popd
pushd ddc && ./bootstrap.sh && make && popd
sed -i "s#REPLACEME/remotebuf#$(pwd)#g" ucb/remotebuf/config/RemoteBuf.java
CXXFLAGS=-DRDMA make java
