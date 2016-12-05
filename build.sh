#!/bin/bash

git submodule update --init
cd javacpp && mvn package && cd ..
sed -i "s#/home/amaro/repos/remotebuf#$(pwd)#g" ucb/remotebuf/config/RemoteBuf.java
make java
