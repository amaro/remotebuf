#include "RemoteBuf.h"

using namespace RemoteBuf;

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
  assert(Size == LocalBuf.size());

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

bool Buffer::writeRemote() {
  std::this_thread::sleep_for(std::chrono::microseconds(5));
  //buf.clear(); TODO: uncomment this and do actual rdma copy
  //std::vector<char>(buf).swap(buf);
  WriteInProgress = false;
  BufferIsRemote = true;
  return true;
}

bool Buffer::readRemote() {
  std::this_thread::sleep_for(std::chrono::microseconds(5));
  // TODO: do actual rdma read
  BufferIsRemote = false;
  return true;
}

/* BufferManager */
BufferManager::BufferManager() {
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
  sstm << "BufferManager::getBuffer(" + id + ") = " << Buffers[id];
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
