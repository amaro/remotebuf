#include "math.h"
#include "RemoteBuf.h"

using namespace RemoteBuf;

/* Buffer */
Buffer::Buffer()
  : WriteInProgress(false), BufferIsRemote(false), Size(0) {
  LocalBuf.reserve(INITIAL_BUFFER_SIZE);
}

Buffer::~Buffer() {}

void Buffer::write(char *buf, unsigned int size) {
  std::stringstream sstm;
  sstm << "Buffer::write(buf, size) on " << this;
  debug(sstm);

  write(buf, size, LocalBuf.end());
}

void Buffer::write(char *buf, unsigned int size, unsigned int offset) {
  std::stringstream sstm;
  sstm << "Buffer::write(buf," << size << "," << offset << ") on " << this;
  debug(sstm);

  write(buf, size, LocalBuf.begin() + offset);
}

void Buffer::write(char *buf, unsigned int size, std::vector<char>::iterator start) {
  assert(Size == LocalBuf.size());

  LocalBuf.insert(start, buf, buf + size);
  Size = LocalBuf.size();
}

void Buffer::flush() {
  assert(Size == LocalBuf.size());

  std::stringstream sstm;
  sstm << "Buffer::flush() on " << this;
  debug(sstm);

  if (WriteInProgress)
    return;

  // transfer async
  WriteInProgress = true;
  WriteFuture = std::async(std::launch::async, &Buffer::writeRemote, this);
}

void Buffer::read(char *buf) {
  std::stringstream sstm;
  sstm << "Buffer::read() on " << this;
  debug(sstm);

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
  std::stringstream sstm;
  sstm << "Buffer::getSize() on " << this << " = " << Size;
  debug(sstm);
  return Size;
}

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

/* BufferManager */
BufferManager::BufferManager() {
	debug("BufferManager::BufferManager()");
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

  std::stringstream sstm;
  sstm << "BufferManager::createBuffer(" + id + ") = " << B;
  debug(sstm);
  return B;
}

Buffer *BufferManager::getBuffer(const std::string id) {
  std::lock_guard<std::mutex> lock(BM);

  if (!bufferExists(id))
    throw std::runtime_error("Buffer doesn't exist");

  std::stringstream sstm;
  sstm << "BufferManager::getBuffer(" << id << ") = " << Buffers[id];
  debug(sstm);
  return Buffers[id];
}

void BufferManager::deleteBuffer(const std::string id) {
  std::lock_guard<std::mutex> lock(BM);

  if (!bufferExists(id))
    throw std::runtime_error("Buffer doesn't exist");

	debug("BufferManager::deleteBuffer(" + id + ")");

  delete Buffers[id];
  Buffers.erase(id);
}

/* assumes lock is held */
bool BufferManager::bufferExists(const std::string id) {
  std::stringstream sstm;
	sstm << "BufferManager::bufferExists(" + id + ") = " << Buffers.count(id);
  debug(sstm);
  return Buffers.count(id) == 1;
}
