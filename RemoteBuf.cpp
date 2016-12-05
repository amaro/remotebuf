#include "RemoteBuf.h"

using namespace RemoteBuf;

/* Buffer */
Buffer::Buffer()
  : WriteInProgress(false), BufferIsRemote(false), Size(0) {}

Buffer::~Buffer() {}

void Buffer::write(char *buf, unsigned int size) {
  write(buf, size, LocalBuf.end());
}

void Buffer::write(char *buf, unsigned int size, unsigned int offset) {
  write(buf, size, LocalBuf.begin() + offset);
}

void Buffer::write(char *buf, unsigned int size, std::vector<char>::iterator start) {
  assert(Size == LocalBuf.size());

  LocalBuf.insert(start, buf, buf + size);
  Size = LocalBuf.size();
}

void Buffer::flush() {
  assert(Size == LocalBuf.size());

  if (WriteInProgress)
    return;

  // transfer async
  WriteInProgress = true;
  WriteFuture = std::async(std::launch::async, &Buffer::writeRemote, this);
}

void Buffer::read(char *buf) {
  assert(Size == LocalBuf.size());

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
BufferManager::BufferManager() { }

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
  return B;
}

Buffer *BufferManager::getBuffer(const std::string id) {
  std::lock_guard<std::mutex> lock(BM);

  if (!bufferExists(id))
    throw std::runtime_error("Buffer doesn't exist");

  return Buffers[id];
}

void BufferManager::deleteBuffer(const std::string id) {
  std::lock_guard<std::mutex> lock(BM);

  if (!bufferExists(id))
    throw std::runtime_error("Buffer doesn't exist");

  delete Buffers[id];
  Buffers.erase(id);
}

/* assumes lock is held */
bool BufferManager::bufferExists(const std::string id) {
  return Buffers.count(id) == 1;
}
