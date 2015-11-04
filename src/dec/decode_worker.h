#ifndef DECODE_WORKER_H
#define DECODE_WORKER_H

#include <nan.h>
#include "buffer_output.h"
#include "../../brotli/dec/decode.h"

class DecodeWorker : public Nan::AsyncWorker {
  public:
    DecodeWorker(Nan::Callback *callback, v8::Local<v8::Object> buffer);
    void Execute();
    void HandleOKCallback();

  private:
    ~DecodeWorker();
    BrotliResult res;
    BrotliMemInput mem_input;
    BrotliInput input;
    std::string mem_output;
};

#endif
