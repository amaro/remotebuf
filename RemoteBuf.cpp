#include "RemoteBuf.h"

using namespace RemoteBuf;

/* Buffer */
Buffer::Buffer()
  : WriteInProgress(false), BufferIsRemote(false), Size(0) {}

Buffer::~Buffer() {}

void Buffer::Write(char *buf, unsigned int size) {
  assert(Size == LocalBuf.size());

  LocalBuf.insert(LocalBuf.end(), buf, buf + size);
  Size = LocalBuf.size();
}

void Buffer::Flush() {
  assert(Size == LocalBuf.size());

  if (WriteInProgress)
    return;

  // transfer async
  WriteInProgress = true;
  WriteFuture = std::async(std::launch::async, &Buffer::WriteRemote, this);
}

void Buffer::Read(char *buf) {
  assert(Size == LocalBuf.size());

  // if a write is in progress, wait for it to finish
  if (WriteInProgress)
    if (!WriteFuture.get())
      throw std::runtime_error("There was an error writting to remote");

  // check if the buffer was pushed to remote
  if (BufferIsRemote)
    // if it was, synchronously read
    if (!ReadRemote())
      throw std::runtime_error("Could not remote read");

  std::copy(LocalBuf.begin(), LocalBuf.end(), buf);
}

unsigned int Buffer::GetSize() {
  return Size;
}

bool Buffer::WriteRemote() {
  std::this_thread::sleep_for(std::chrono::microseconds(5));
  //buf.clear(); TODO: uncomment this and do actual rdma copy
  //std::vector<char>(buf).swap(buf);
  WriteInProgress = false;
  BufferIsRemote = true;
  return true;
}

bool Buffer::ReadRemote() {
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

Buffer *BufferManager::CreateBuffer(const std::string id) {
  if (BufferExists(id))
    throw std::runtime_error("Buffer already exists");

  Buffer *B = new Buffer();
  Buffers.insert({id, B});
  return B;
}

Buffer *BufferManager::GetBuffer(const std::string id) {
  if (!BufferExists(id))
    throw std::runtime_error("Buffer doesn't exist");

  return Buffers[id];
}

void BufferManager::DeleteBuffer(const std::string id) {
  if (!BufferExists(id))
    throw std::runtime_error("Buffer doesn't exist");

  delete Buffers[id];
  Buffers.erase(id);
}

bool BufferManager::BufferExists(const std::string id) {
  return Buffers.count(id) == 1;
}
