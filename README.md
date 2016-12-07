# remotebuf

Build for the first time using

    ./build.sh

After that, run:

    make java

If you want to build with a custom compiler (doesn't work for jni .so generation), do:
    CXX=customgcc make java

If you want to build with logging output do:
    CXXFLAGS=-DDEBUG make java
