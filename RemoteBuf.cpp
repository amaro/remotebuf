#include "RemoteBuf.h"

using namespace RemoteBuf;

/* BufferManager */
BufferManager::BufferManager() {
#ifdef DEBUG
	debug("BufferManager::BufferManager()");
#endif
}

BufferManager::~BufferManager() {
#ifdef DEBUG
	debug("BufferManager::~BufferManager()");
#endif
}

void BufferManager::write(const std::string id, char *buf, unsigned int s) {
    Client.connect("10.10.49.98", "12345");

    sirius::FileAllocRec alloc1 = Client.allocate(id, s);

    Client.write_sync(alloc1, 0, s, buf);
}

void BufferManager::read(char *buf) {
    Client.connect("10.10.49.98", "12345");
    Client.read_sync(read_alloc, 0, read_alloc->size, buf);
}

unsigned int BufferManager::get_read_alloc(const std::string id) {
    Client.connect("10.10.49.98", "12345");
    read_alloc = Client.allocate(id, 0); // does the size matter here?
    return read_alloc->size;
}

