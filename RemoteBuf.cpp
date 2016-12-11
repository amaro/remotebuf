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

void BufferManager::read(const std::string id, char *buf, unsigned int s) {
    Client.connect("10.10.49.98", "12345");

    sirius::FileAllocRec alloc1 = Client.allocate(id, s); // does the size matter here?

    // TODO need the size here to read
    Client.read_sync(alloc1, 0, s, buf);
}
