#!/bin/bash

git submodule update --init
cd javacpp && mvn clean package && cd ..
sed -i "s#/home/amaro/repos/spark/external/remotebuf#$(pwd)#g" ucb/remotebuf/config/RemoteBuf.java
make java
