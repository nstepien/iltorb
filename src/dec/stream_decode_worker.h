#ifndef STREAM_DECODE_WORKER_H
#define STREAM_DECODE_WORKER_H

#include <nan.h>
#include "stream_decode.h"
#include "../../brotli/dec/decode.h"

class StreamDecodeWorker : public Nan::AsyncWorker {
 public:
  StreamDecodeWorker(Nan::Callback *callback, StreamDecode* obj, bool finish);

  void Execute();
  void HandleOKCallback();

  private:
    StreamDecode* obj;
    BrotliResult res;
    bool finish;
};

#endif
