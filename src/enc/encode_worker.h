#ifndef ENCODE_WORKER_H
#define ENCODE_WORKER_H

#include <nan.h>
#include "buffer_out.h"
#include "../../brotli/enc/encode.h"

class EncodeWorker : public Nan::AsyncWorker {
 public:
  EncodeWorker(Nan::Callback *callback, brotli::BrotliParams params, brotli::BrotliMemIn input);
  void Execute();
  void HandleOKCallback();

  private:
    ~EncodeWorker();
    bool res;
    brotli::BrotliParams params;
    brotli::BrotliMemIn input;
    BufferOut output;
};

#endif
