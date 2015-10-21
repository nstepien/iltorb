#ifndef STREAM_ENCODE_WORKER_H
#define STREAM_ENCODE_WORKER_H

#include <nan.h>
#include "../brotli/enc/encode.h"

class StreamEncodeWorker : public Nan::AsyncWorker {
 public:
  StreamEncodeWorker(Nan::Callback *callback, brotli::BrotliCompressor *compressor, bool is_last);

  void Execute();
  void HandleOKCallback();

  private:
    brotli::BrotliCompressor *compressor;
    bool is_last;
    bool res;
    size_t output_size = 0;
    char* output_buffer;
};

#endif
