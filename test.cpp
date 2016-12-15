#include "RemoteBuf.h"
#include <string.h>
#include <iostream>

#undef NDEBUG
#include <cassert>

using namespace RemoteBuf;

int main(int argc, char *argv[]) {
  BufferManager BM("10.10.49.90", "12345");
  Buffer *B = BM.createBuffer("hi");
  assert(BM.bufferExists("hi"));

  char buf[] = "hello";
  B->write(buf, strlen(buf));
  B->flush();

  B = BM.getBuffer("hi");
  char *buf2 = new char[B->getSize()];
  B->read(buf2);

  std::cout << "read: " << buf2 << "\n";

  if (memcmp("hellohello", buf2, B->getSize()) != 0) {
    std::cout << "strings not equal\n";
    return 1;
  }

  delete[] buf2;

  Buffer *B2 = BM.createBuffer("hi2");
  assert(BM.bufferExists("hi2"));
  B2->write(buf, strlen(buf));
  B2->flush();
  BM.deleteBuffer("hi2");
  assert(!BM.bufferExists("hi2"));

  std::cout << "test passed\n";
  return 0;
}
