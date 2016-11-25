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

    /*
    Writes buf with size s to LocalBuf
    */
    void Write(char *buf, unsigned int s);

    /*
    Called after writing is done
    */
    void WriteDone();

    /*
    Copy LocalBuf to buf
    */
    void Read(char *buf);

    /*
    Get the size of the buffer, regardless of where it is located.
    */
    unsigned int GetSize();
  private:
    std::vector<char> LocalBuf;
    bool WriteBegan;
    unsigned int Size;
    std::future<bool> WriteFuture;

    bool WriteRemote(std::vector<char> &buf);
    bool ReadRemote(std::vector<char> &buf);
};

/* BufferManager not reentrant */
class BufferManager {
  public:
    BufferManager();
    ~BufferManager();

    Buffer *CreateBuffer(const std::string id);
    Buffer *GetBuffer(const std::string id);
    bool BufferExists(const std::string id);
  private:
    std::unordered_map<std::string, Buffer *> Buffers;
};
}
