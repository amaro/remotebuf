#include "RemoteBuf.h"
#include <string.h>
#include <iostream>

#undef NDEBUG
#include <cassert>

using namespace RemoteBuf;

int main(int argc, char *argv[]) {
  BufferManager BM;
  Buffer *B = BM.CreateBuffer("hi");
  assert(BM.BufferExists("hi"));

  char buf[] = "hello";
  B->Write(buf, strlen(buf));
  B->Write(buf, strlen(buf));
  B->WriteDone();

  B = BM.GetBuffer("hi");
  char *buf2 = new char[B->GetSize()];
  B->Read(buf2);

  if (memcmp("hellohello", buf2, B->GetSize()) != 0) {
    std::cout << "strings not equal\n";
    return 1;
  }

  delete[] buf2;

  Buffer *B2 = BM.CreateBuffer("hi2");
  assert(BM.BufferExists("hi2"));
  B2->Write(buf, strlen(buf));
  B2->WriteDone();
  BM.DeleteBuffer("hi2");
  assert(!BM.BufferExists("hi2"));

  std::cout << "test passed\n";
  return 0;
}
