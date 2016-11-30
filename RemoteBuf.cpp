#include "RemoteBuf.h"

using namespace RemoteBuf;

/* Buffer */
Buffer::Buffer()
  : WriteBegan(false), Size(0) {}

Buffer::~Buffer() {}

void Buffer::Write(char *buf, unsigned int size) {
  assert(Size == LocalBuf.size());

  LocalBuf.insert(LocalBuf.end(), buf, buf + size);
  Size = LocalBuf.size();
}

void Buffer::WriteDone() {
  assert(Size == LocalBuf.size());

  // transfer async
  WriteBegan = true;
  WriteFuture = std::async(std::launch::async, &Buffer::WriteRemote, this, std::ref(LocalBuf));
}

void Buffer::Read(char *buf) {
  assert(WriteBegan);

  // check if the async write was successful
  if (!WriteFuture.get()) {
    throw std::runtime_error("There was an error writting to remote");
  }

  // synchronously read
  if (!ReadRemote(LocalBuf)) {
    throw std::runtime_error("Could not remote read");
  }

  std::copy(LocalBuf.begin(), LocalBuf.end(), buf);
  WriteBegan = false;
}

unsigned int Buffer::GetSize() {
  return Size;
}

bool Buffer::WriteRemote(std::vector<char> &buf) {
  std::this_thread::sleep_for(std::chrono::microseconds(5));
  //buf.clear(); TODO: uncomment this and do actual rdma copy
  //std::vector<char>(buf).swap(buf);
  return true;
}

bool Buffer::ReadRemote(std::vector<char> &buf) {
  std::this_thread::sleep_for(std::chrono::microseconds(5));
  // TODO: do actual rdma read
  return true;
}

/* BufferManager */
BufferManager::BufferManager() { }

BufferManager::~BufferManager() {
  for (auto B: Buffers) {
    delete B.second;
  }
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
