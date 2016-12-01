#include <vector>
#include <string>
#include <future>
#include <unordered_map>
#include <cassert>
#include <stdexcept>

namespace RemoteBuf {
/* Buffer is not reentrant */
class Buffer {
  public:
    Buffer();
    ~Buffer();

    /* Writes buf with size s to LocalBuf */
    void Write(char *buf, unsigned int s);

    /* Flush pending requests (write) */
    void Flush();

    /* Copy LocalBuf to buf */
    void Read(char *buf);

    /* Get the size of the buffer, regardless of where it is located. */
    unsigned int GetSize();
  private:
    std::vector<char> LocalBuf;
    bool WriteInProgress;
    bool BufferIsRemote;
    unsigned int Size;
    std::future<bool> WriteFuture;

    bool WriteRemote();
    bool ReadRemote();
};

/* BufferManager not reentrant */
class BufferManager {
  public:
    BufferManager();
    ~BufferManager();

    /* Creates buffer with specified id.
     * Throws an exception if the buffer already exists
     */
    Buffer *CreateBuffer(const std::string id);

    /* Gets the buffer with specified id.
     * Throws an exception if the buffer doesn't exist
     */
    Buffer *GetBuffer(const std::string id);

    /* If the buffer exists, it is deleted.
     * If it doesn't, an exception is thrown.
     */
    void DeleteBuffer(const std::string id);

    /* Returns true if the buffer with specified id exists. */
    bool BufferExists(const std::string id);
  private:
    std::unordered_map<std::string, Buffer *> Buffers;
};
}
