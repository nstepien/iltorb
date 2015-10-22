#ifndef BUFFER_OUT_H
#define BUFFER_OUT_H

#include "../../brotli/enc/encode.h"

class BufferOut : public brotli::BrotliOut {
  public:
    bool Write(const void* buf, size_t n);
    std::string buffer;
};

#endif
