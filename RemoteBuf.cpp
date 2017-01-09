#include "RemoteBuf.h"
#include <unistd.h>
#include <limits.h>

using namespace RemoteBuf;

/* Global Performance Counters */
typedef std::chrono::high_resolution_clock::time_point timeStamp_t;
static inline timeStamp_t getTimeStamp() {
  return std::chrono::high_resolution_clock::now();
}

static inline int64_t getElapsedTime(timeStamp_t start, timeStamp_t end) {
  return (end - start).count();
}

// timeStamp_t stat_remoteWriteTime; #<{(| Time spent writing in ns |)}>#
// timeStamp_t stat_remoteReadTime;  #<{(| Time spent Reading in ns |)}>#
/* Time spent reading/writing over RDMA (note: write times are async) */
int64_t stat_remoteWriteTime = 0;
int64_t stat_remoteReadTime = 0;

/* Time waiting for a write to finish (may happen during read) */ 
int64_t stat_waitOnWrite = 0; 

int64_t stat_bytesWritten = 0;
int64_t stat_bytesRead = 0;

/* Buffer */
Buffer::Buffer(const std::string &Serv, const std::string &Port)
  : WriteInProgress(false), BufferIsRemote(false), Size(0), RdmaServ(Serv),
  RdmaPort(Port) {

  /* setup logging */
  char logPrefix[HOST_NAME_MAX];
  int res = gethostname(logPrefix, HOST_NAME_MAX);
  assert(res == 0);

  std::string logPath = "~/spark_results/";
  // std::string logPath = "~/";
  logPath.append(logPrefix);
  logOut.open(logPath.c_str());
  logOut << "Opened RmemLog";

  WriteBuf.reserve(INITIAL_BUFFER_SIZE);
  Client.connect(RdmaServ, RdmaPort);
}

Buffer::~Buffer() {
  // if read was never called, and the object is being destroyed
  // before the write is done, wait for the write to be done,
  // otherwise there will be transient errors.
  if (WriteInProgress)
    if (!WriteFuture.get())
      assert("Cannot exit before write is done");
}

void Buffer::write(char *buf, unsigned int size) {
  std::stringstream sstm;
  sstm << "Buffer::write(buf, size) on " << this;
  debug(sstm);

  write(buf, size, WriteBuf.end());
}

void Buffer::write(char *buf, unsigned int size, unsigned int offset) {
  std::stringstream sstm;
  sstm << "Buffer::write(buf," << size << "," << offset << ") on " << this;
  debug(sstm);

  write(buf, size, WriteBuf.begin() + offset);
}

void Buffer::write(char *buf, unsigned int size, std::vector<char>::iterator start) {
  assert(Size == WriteBuf.size());

  
  WriteBuf.insert(start, buf, buf + size);
  Size = WriteBuf.size();
  
}

void Buffer::flush() {
  assert(Size == WriteBuf.size());

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
  if (WriteInProgress) {
    timeStamp_t start = getTimeStamp();
    if (!WriteFuture.get())
      throw std::runtime_error("There was an error writting to remote");
    stat_waitOnWrite += getElapsedTime(start, getTimeStamp());
  }

  // check if the buffer was pushed to remote
  if (BufferIsRemote)
    // if it was, synchronously read
    if (!readRemote(buf))
      throw std::runtime_error("Could not remote read");
}

unsigned int Buffer::getSize() {
  std::stringstream sstm;
  sstm << "Buffer::getSize() on " << this << " = " << Size;
  debug(sstm);
  return Size;
}

bool Buffer::writeRemote() {
  timeStamp_t start = getTimeStamp();

  Alloc = Client.allocate(Size);

  // wrap local buffer into RDMAMem
  sirius::RDMAMem wrap(&WriteBuf[0], Size);

  if (!Client.write_sync(Alloc, 0, Size, &WriteBuf[0], &wrap)) {
    throw std::runtime_error("Error while doing write_sync");
  }

  // clear local buffer
  WriteBuf.clear();
  std::vector<char>(WriteBuf).swap(WriteBuf);

  // management stuff
  WriteInProgress = false;
  BufferIsRemote = true;

  stat_remoteWriteTime += getElapsedTime(start, getTimeStamp());

  // done
  return true;
}

bool Buffer::readRemote(char *buf) {
  assert(BufferIsRemote);

  timeStamp_t start = getTimeStamp();

  // wrap local buffer into RDMAMem
  sirius::RDMAMem wrap(buf, Size);

  // read from server
  if (!Client.read_sync(Alloc, 0, Size, buf, &wrap)) {
    throw std::runtime_error("Error while doing read_sync");
  }

  stat_remoteReadTime += getElapsedTime(start, getTimeStamp());

  return true;
}

/* BufferManager */
BufferManager::BufferManager(std::string Serv, std::string Port)
  : RdmaServ(Serv), RdmaPort(Port) {
	debug("BufferManager::BufferManager()");
}

BufferManager::~BufferManager() {
  std::cout << "BufferManager Destructor\n";
  for (auto B: Buffers)
    delete B.second;
}

Buffer *BufferManager::createBuffer(const std::string id) {
  std::lock_guard<std::mutex> lock(BM);

  if (bufferExists(id))
    throw std::runtime_error("Buffer already exists");

  Buffer *B = new Buffer(RdmaServ, RdmaPort);
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

/* Divide nanos by this to get Millis */
#define convToMillis ((double)1E6)
std::string BufferManager::reportStats() {
  ReportString.clear();
  ReportString += "rbuf_remoteWriteTime: " + std::to_string(stat_remoteWriteTime / convToMillis) + "\n";
  ReportString += "rbuf_remoteReadTime: " + std::to_string(stat_remoteReadTime / convToMillis) + "\n";
  ReportString += "rbuf_waitOnWrite: " + std::to_string(stat_waitOnWrite / convToMillis) + "\n";
  // std::cout << "Internal BM Stat Report: \n" + ReportString;
  return ReportString;
}
