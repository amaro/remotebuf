CXXFLAGS += -Wall -Werror -std=c++11
JAVACPPPATH := ./javacpp/target
JAVACPPJAR := $(JAVACPPPATH)/javacpp.jar
PKGPATH := ucb/remotebuf

lib: libRemoteBuf.so

libRemoteBuf.so: RemoteBuf.cpp RemoteBuf.h
	g++-6 $(CXXFLAGS) -fPIC -c RemoteBuf.cpp -o RemoteBuf.o
	g++-6 $(CXXFLAGS) -shared -Wl,-soname,libRemoteBuf.so -o libRemoteBuf.so RemoteBuf.o -pthread

test: libRemoteBuf.so test.cpp
	g++-6 $(CXXFLAGS) -L. test.cpp -o test -lRemoteBuf

java: libRemoteBuf.so TestRemoteBuf.class remoteMem.jar

TestRemoteBuf.class: TestRemoteBuf.java remoteMem.jar
	javac -cp $(JAVACPPJAR):remoteMem.jar $<

remoteMem.jar: $(PKGPATH)/libjniRemoteBufConfig.so $(PKGPATH)/RemoteBuf.class $(PKGPATH)/ROutputStream.class $(PKGPATH)/RWritableByteChannel.class $(PKGPATH)/config/RemoteBuf.class
	jar cf remoteMem.jar $(PKGPATH)/*.class $(PKGPATH)/config/*.class

$(PKGPATH)/libjniRemoteBufConfig.so: $(PKGPATH)/RemoteBuf.class
	cp utils.h $(PKGPATH)
	cp RemoteBuf.h $(PKGPATH)
	java -jar $(JAVACPPJAR) $(PKGPATH)/RemoteBuf

$(PKGPATH)/ROutputStream.class: $(PKGPATH)/ROutputStream.java
	javac -cp $(JAVACPPJAR):. $<

$(PKGPATH)/RWritableByteChannel.class: $(PKGPATH)/RWritableByteChannel.java
	javac -cp $(JAVACPPJAR):. $<

$(PKGPATH)/RemoteBuf.class: $(PKGPATH)/RemoteBuf.java
	javac -cp $(JAVACPPJAR):. $<

# this is the automatically generated RemoteBuf java interface
$(PKGPATH)/RemoteBuf.java: $(PKGPATH)/config/RemoteBuf.class
	java -jar $(JAVACPPJAR) $<

# this is the first class that has to be built
$(PKGPATH)/config/RemoteBuf.class: $(PKGPATH)/config/RemoteBuf.java
	javac -cp $(JAVACPPJAR) $<

clean:
	rm -rf *.o *.so *.class *.jar $(PKGPATH)/*.h $(PKGPATH)/*.class $(PKGPATH)/config/*.class $(PKGPATH)/linux-x86_64 test
