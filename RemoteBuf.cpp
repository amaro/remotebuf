#include "RemoteBuf.h"

using namespace RemoteBuf;

/* BufferManager */
BufferManager::BufferManager() {
#ifdef DEBUG
	debug("BufferManager::BufferManager()");
#endif
    isConnected = false;
}

BufferManager::~BufferManager() {
#ifdef DEBUG
	debug("BufferManager::~BufferManager()");
#endif
}

void BufferManager::write(const std::string id, char *buf, unsigned int s) {
    std::lock_guard<std::mutex> lock(BM);

    // TODO make this read from a file:

    if (!isConnected) {
        Client.connect("10.10.49.98", "12345");
        isConnected = true;
    }
    sirius::FileAllocRec alloc1 = Client.allocate(id, s);
    Client.write_sync(alloc1, 0, s, buf);
}

void BufferManager::write_file(const std::string fname, const std::string id) {
    std::lock_guard<std::mutex> lock(BM);

    if (!isConnected) {
        Client.connect("10.10.49.98", "12345");
        isConnected = true;
    }

    std::ifstream is (fname, std::ifstream::binary);
    if (is) {
        is.seekg(0, is.end);
        int length = is.tellg();
        is.seekg(0, is.beg);

        printf("alloc-ing array for data file write\n");
        char *buf = (char*)malloc(length); //new char[length];
        printf("done alloc-ing array for data file write\n");
        is.read(buf, length);
        printf("done reading from file, now sending alloc rec\n");
        sirius::FileAllocRec alloc1 = Client.allocate(id, length);
        printf("got alloc rec, now writing data\n");
        Client.write_sync(alloc1, 0, length, buf);
        printf("done writing data\n");
        free(buf);
    } else {
        printf("FAIL\n");
    }
}

void BufferManager::read(const std::string id, char *buf, unsigned int s) {
    std::lock_guard<std::mutex> lock(BM);

    if (!isConnected) {
        Client.connect("10.10.49.98", "12345");
        isConnected = true;
    }

    sirius::FileAllocRec alloc1 = Client.allocate(id, s);
    Client.read_sync(alloc1, 0, s, buf);
}

unsigned int BufferManager::get_read_alloc(const std::string id) {
    std::lock_guard<std::mutex> lock(BM);

    if (!isConnected) {
        Client.connect("10.10.49.98", "12345");
        isConnected = true;
    }

    printf("alloc-ing: %s\n", id.c_str());

//    Client.connect("10.10.49.98", "12345");
    read_alloc = Client.allocate(id, 0); // does the size matter here?
    printf("check alloc-ing: %s\n", id.c_str());

    return read_alloc->size;
}

