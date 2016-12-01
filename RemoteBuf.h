#include <vector>
#include <string>
#include <future>
#include <unordered_map>
#include <cassert>
#include <stdexcept>
#include <mutex>

namespace RemoteBuf {
/* Buffer is not reentrant */
class Buffer {
  public:
    Buffer();
    ~Buffer();

    /* Writes buf with size s to LocalBuf */
    void write(char *buf, unsigned int s);

    /* Flush pending requests (write) */
    void flush();

    /* Copy LocalBuf to buf */
    void read(char *buf);

    /* Get the size of the buffer, regardless of where it is located. */
    unsigned int getSize();
  private:
    std::vector<char> LocalBuf;
    bool WriteInProgress;
    bool BufferIsRemote;
    unsigned int Size;
    std::future<bool> WriteFuture;

    bool writeRemote();
    bool readRemote();
};

/* BufferManager not reentrant */
class BufferManager {
  public:
    BufferManager();
    ~BufferManager();

    /* Creates buffer with specified id.
     * Throws an exception if the buffer already exists
     */
    Buffer *createBuffer(const std::string id);

    /* Gets the buffer with specified id.
     * Throws an exception if the buffer doesn't exist
     */
    Buffer *getBuffer(const std::string id);

    /* If the buffer exists, it is deleted.
     * If it doesn't, an exception is thrown.
     */
    void deleteBuffer(const std::string id);

    /* Returns true if the buffer with specified id exists. */
    bool bufferExists(const std::string id);
  private:
    std::unordered_map<std::string, Buffer *> Buffers;
    std::mutex BM;
};
}
