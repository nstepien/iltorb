#include "stream_coder.h"

StreamCoder::StreamCoder() {
}

StreamCoder::~StreamCoder() {
  size_t n_chunks = pending_output.size();
  for (size_t i = 0; i < n_chunks; i++)
    alloc.Free(pending_output[i]);

  alloc.ReportMemoryToV8();
}
