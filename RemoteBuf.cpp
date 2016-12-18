#include "RemoteBuf.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#define IP ("10.10.49.85")

using namespace RemoteBuf;

/* BufferManager */
BufferManager::BufferManager() {
    printf("THIS IS BUFFER MANAGER FROM 12-15-16:5:03pm\n");
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

    if (!isConnected) {
        Client.connect(IP, "12345");
        isConnected = true;
    }
    sirius::FileAllocRec alloc1 = Client.allocate(id, s);
    Client.write_sync(alloc1, 0, s, buf);
}

void BufferManager::write_file(const std::string fname, const std::string id) {
    std::lock_guard<std::mutex> lock(BM);

    if (!isConnected) {
        Client.connect(IP, "12345");
        isConnected = true;
    }
    char * memblock;
    int fd;
    struct stat sb;
    fd = open(fname.c_str(), O_RDONLY);
    fstat(fd, &sb);
    memblock = (char*)mmap(NULL, sb.st_size, PROT_READ, MAP_SHARED, fd, 0);

    sirius::FileAllocRec alloc1 = Client.allocate(id, sb.st_size);
    Client.write_sync(alloc1, 0, sb.st_size, memblock);

    munmap(memblock, sb.st_size);
}

void BufferManager::read(const std::string id, char *buf, unsigned int s) {
    std::lock_guard<std::mutex> lock(BM);

    if (!isConnected) {
        Client.connect(IP, "12345");
        isConnected = true;
    }
    sirius::FileAllocRec alloc1 = Client.allocate(id, s);
    Client.read_sync(alloc1, 0, s, buf);
}

void BufferManager::read_offset(const std::string id, char *buf, unsigned int s, unsigned int offset) {
    std::lock_guard<std::mutex> lock(BM);

    if (!isConnected) {
        Client.connect(IP, "12345");
        isConnected = true;
    }
    sirius::FileAllocRec alloc1 = Client.allocate(id, s);
    Client.read_sync(alloc1, offset, s, buf);
}

/*unsigned int BufferManager::get_read_alloc(const std::string id) {
    std::lock_guard<std::mutex> lock(BM);

    if (!isConnected) {
        Client.connect(IP, "12345");
        isConnected = true;
    }
    sirius::FileAllocRec alloc1 = Client.allocate(id, 0); // does the size matter here?
    return alloc1->size;
}*/
