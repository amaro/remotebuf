CXXFLAGS += -Wall -Werror
JAVACPPPATH := /home/amaro/javardma/javacpp/target
JAVACPPJAR := $(JAVACPPPATH)/javacpp.jar

lib: libRemoteBuf.so

libRemoteBuf.so: RemoteBuf.cpp RemoteBuf.h
	g++ $(CXXFLAGS) -fPIC -c RemoteBuf.cpp -o RemoteBuf.o
	g++ $(CXXFLAGS) -shared -Wl,-soname,libRemoteBuf.so -o libRemoteBuf.so RemoteBuf.o -pthread

test: libRemoteBuf.so test.cpp
	g++ $(CXXFLAGS) -L. test.cpp -o test -lRemoteBuf

java: libRemoteBuf.so TestRemoteBuf.class remoteMem.jar

remoteMem.jar: libjniRemoteBufConfig.so RemoteBuf.class ROutputStream.class RWritableByteChannel.class
	jar cf remoteMem.jar RemoteBuf.class ROutputStream.class RWritableByteChannel.class

libjniRemoteBufConfig.so: RemoteBuf.class
	java -jar $(JAVACPPJAR) RemoteBuf

TestRemoteBuf.class: RemoteBuf.class ROutputStream.class RWritableByteChannel.class TestRemoteBuf.java
	javac -cp $(JAVACPPJAR):. TestRemoteBuf.java

ROutputStream.class: ROutputStream.java
	javac -cp $(JAVACPPJAR):. ROutputStream.java

RWritableByteChannel.class: RWritableByteChannel.java
	javac -cp $(JAVACPPJAR):. RWritableByteChannel.java

RemoteBuf.class: RemoteBuf.java
	javac -cp $(JAVACPPJAR):. RemoteBuf.java

# this is the automatically generated RemoteBuf java interface
RemoteBuf.java: RemoteBufConfig.class
	java -jar $(JAVACPPJAR) RemoteBufConfig

# this is the first class that has to be built
RemoteBufConfig.class: RemoteBufConfig.java
	javac -cp $(JAVACPPJAR) RemoteBufConfig.java

clean:
	rm -rf *.o *.so *.class linux-x86_64 test
