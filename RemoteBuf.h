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

/* BufferManager not reentrant */
class BufferManager {
  public:
    BufferManager(int ipselect);
    ~BufferManager();

    void write(const std::string id, char *buf, unsigned int s);
    void write_file(const std::string fname, const std::string id);
    void read(const std::string id, char *buf, unsigned int s);
    void read_offset(const std::string id, char *buf, unsigned int s, unsigned int offset);
//    unsigned int get_read_alloc(const std::string id);

  private:
    sirius::BladeFileClient Client;
    bool isConnected;
    std::mutex BM;
    const char* myip;
};

}
