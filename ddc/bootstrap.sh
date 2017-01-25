# get third party libs
git submodule init 
git submodule update

# compile libcuckoo
cd third_party/libcuckoo
autoreconf -fis
CXX=g++-6 ./configure
make -j 10

# compile sparsehash
cd ../sparsehash
CXX=g++-6 ./configure
make -j 10
cd ../..

# main compilation
autoreconf
automake --add-missing
CXX=g++-6 ./configure
