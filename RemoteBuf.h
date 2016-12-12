#include <vector>
#include <string>
#include <future>
#include <unordered_map>
#include <cassert>
#include <stdexcept>
#include <mutex>
#include <src/client/BladeFileClient.h>
#include <src/common/FileAllocationRecord.h>
#include "utils.h"

namespace RemoteBuf {

const int INITIAL_BUFFER_SIZE = 16384;
const char RDMA_PORT[] = "12345";
const char RDMA_ADDR[] = "10.10.49.94";

/* BufferManager not reentrant */
class BufferManager {
  public:
    BufferManager();
    ~BufferManager();

    void write(const std::string id, char *buf, unsigned int s);
    void read(char *buf);
    unsigned int get_read_alloc(const std::string id);

  private:
    sirius::BladeFileClient Client;
    sirius::FileAllocRec read_alloc;
};

}
