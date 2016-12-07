#!/bin/bash

git submodule update --init
cd javacpp && mvn --global-settings /scratch/sagark/settings.xml clean package && cd ..
sed -i "s#/home/amaro/repos/spark/external/remotebuf#$(pwd)#g" ucb/remotebuf/config/RemoteBuf.java
sed -i "s#g++#g++-6#g" javacpp/src/main/resources/org/bytedeco/javacpp/properties/linux-x86_64.properties
make java
