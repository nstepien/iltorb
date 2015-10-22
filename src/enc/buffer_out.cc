#include "buffer_out.h"

bool BufferOut::Write(const void* buf, size_t n) {
  buffer.append((char*) buf, n);
  return true;
}
