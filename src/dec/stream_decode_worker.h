#ifndef STREAM_DECODE_WORKER_H
#define STREAM_DECODE_WORKER_H

#include <nan.h>
#include "stream_decode.h"

using namespace v8;
using namespace Nan;

class StreamDecodeWorker : public AsyncProgressQueueWorker<uint8_t> {
  public:
    StreamDecodeWorker(Callback *callback, StreamDecode* obj);

    void Execute(const ExecutionProgress& progress);
    void HandleProgressCallback(const uint8_t *data, size_t size);

  private:
    ~StreamDecodeWorker();
    StreamDecode* obj;
};

#endif
