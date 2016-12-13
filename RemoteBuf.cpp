#include "math.h"
#include "RemoteBuf.h"

using namespace RemoteBuf;

/* Simulated Latency/Bandwidth of RMEM in us and Bytes/us */
//#define RMEM_LAT 0
//#define RMEM_LAT 1
#define RMEM_LAT 5
//#define RMEM_LAT 10
//#define RMEM_LAT 100

//#define RMEM_BW 12 /* ~100 Mb */
//#define RMEM_BW 120 /* 1 Gb/S */
#define RMEM_BW 1200  /* 10 Gb/S */
// #define RMEM_BW 5000  /* 40 Gb/S */
//#define RMEM_BW 12000 /* 100 Gb/S*/
//#define RMEM_BW 0 /* INF */

/* Buffer */
Buffer::Buffer()
  : WriteInProgress(false), BufferIsRemote(false), Size(0) {
  LocalBuf.reserve(INITIAL_BUFFER_SIZE);
}

Buffer::~Buffer() {}

void Buffer::write(char *buf, unsigned int size) {
#ifdef DEBUG
  std::stringstream sstm;
  sstm << "Buffer::write(buf, size) on " << this;
  debug(sstm);
#endif

  write(buf, size, LocalBuf.end());
}

void Buffer::write(char *buf, unsigned int size, unsigned int offset) {
#ifdef DEBUG
  std::stringstream sstm;
  sstm << "Buffer::write(buf," << size << "," << offset << ") on " << this;
  debug(sstm);
#endif

  write(buf, size, LocalBuf.begin() + offset);
}

void Buffer::write(char *buf, unsigned int size, std::vector<char>::iterator start) {
  assert(Size == LocalBuf.size());

  LocalBuf.insert(start, buf, buf + size);
  Size = LocalBuf.size();
}

void Buffer::flush() {
  assert(Size == LocalBuf.size());

#ifdef DEBUG
  std::stringstream sstm;
  sstm << "Buffer::flush() on " << this;
  debug(sstm);
#endif

  if (WriteInProgress)
    return;

  // transfer async
  WriteInProgress = true;
  WriteFuture = std::async(std::launch::async, &Buffer::writeRemote, this);
}

void Buffer::read(char *buf) {
#ifdef DEBUG
  std::stringstream sstm;
  sstm << "Buffer::read() on " << this;
  debug(sstm);
#endif

  // if a write is in progress, wait for it to finish
  if (WriteInProgress)
    if (!WriteFuture.get())
      throw std::runtime_error("There was an error writting to remote");

  // check if the buffer was pushed to remote
  if (BufferIsRemote)
    // if it was, synchronously read
    if (!readRemote())
      throw std::runtime_error("Could not remote read");

  std::copy(LocalBuf.begin(), LocalBuf.end(), buf);
}

unsigned int Buffer::getSize() {
#ifdef DEBUG
  std::stringstream sstm;
  sstm << "Buffer::getSize() on " << this << " = " << Size;
  debug(sstm);
#endif
  return Size;
}

#ifdef RDMA
bool Buffer::writeRemote() {
  // write to server
  Client.connect(RDMA_ADDR, RDMA_PORT);
  Alloc = Client.allocate(Size);
  Client.write_sync(Alloc, 0, Size, &LocalBuf[0]);

  // clear local buffer
  LocalBuf.clear();
  std::vector<char>(LocalBuf).swap(LocalBuf);

  // management stuff
  WriteInProgress = false;
  BufferIsRemote = true;

  // done
  return true;
}

bool Buffer::readRemote() {
  assert(BufferIsRemote);

  // reserve local space
  LocalBuf.resize(Size);

  // read from server
  Client.read_sync(Alloc, 0, Size, &LocalBuf[0]);

  // management stuff
  BufferIsRemote = false;
  return true;
}
#else // for testing purposes only
bool Buffer::writeRemote() {
  WriteInProgress = false;
  BufferIsRemote = true;
  return true;
}

bool Buffer::readRemote() {
  assert(BufferIsRemote);

  /* Simulate Remote Memory latency/bandwidth (rounded to nearest us) */
  #if RMEM_BW == 0
    int64_t waitTime = RMEM_LAT;
  #else
    int64_t waitTime = RMEM_LAT + std::ceil((double)Size / RMEM_BW);
  #endif
#ifdef DEBUG
  std::stringstream sstm;
  sstm << "Buffer::readRemote() waiting " << waitTime << " us to write " << Size << " bytes";
  debug(sstm);
#endif
  std::this_thread::sleep_for(std::chrono::microseconds(waitTime));

  BufferIsRemote = false;
  return true;
}
#endif

/* BufferManager */
BufferManager::BufferManager() {
#ifdef RDMA
  std::cout << "RMEM using RDMA 1\n";
#else
  std::cout << "RMEM using local memory (testing)\n";
#endif

#ifdef DEBUG
	debug("BufferManager::BufferManager()");
#endif
}

BufferManager::~BufferManager() {
  for (auto B: Buffers)
    delete B.second;
}

Buffer *BufferManager::createBuffer(const std::string id) {
  std::lock_guard<std::mutex> lock(BM);

  if (bufferExists(id))
    throw std::runtime_error("Buffer already exists");

  Buffer *B = new Buffer();
  Buffers.insert({id, B});
#ifdef DEBUG
  std::stringstream sstm;
  sstm << "BufferManager::createBuffer(" + id + ") = " << B;
  debug(sstm);
#endif
  return B;
}

Buffer *BufferManager::getBuffer(const std::string id) {
  std::lock_guard<std::mutex> lock(BM);

  if (!bufferExists(id))
    throw std::runtime_error("Buffer doesn't exist");

#ifdef DEBUG
  std::stringstream sstm;
  sstm << "BufferManager::getBuffer(" << id << ") = " << Buffers[id];
  debug(sstm);
#endif
  return Buffers[id];
}

void BufferManager::deleteBuffer(const std::string id) {
  std::lock_guard<std::mutex> lock(BM);

  if (!bufferExists(id))
    throw std::runtime_error("Buffer doesn't exist");

#ifdef DEBUG
	debug("BufferManager::deleteBuffer(" + id + ")");
#endif
  delete Buffers[id];
  Buffers.erase(id);
}

/* assumes lock is held */
bool BufferManager::bufferExists(const std::string id) {
#ifdef DEBUG
  std::stringstream sstm;
	sstm << "BufferManager::bufferExists(" + id + ") = " << Buffers.count(id);
  debug(sstm);
#endif
  return Buffers.count(id) == 1;
}
