#ifndef STREAM_ENCODE_WORKER_H
#define STREAM_ENCODE_WORKER_H

#include <nan.h>
#include "stream_encode.h"

using namespace v8;
using namespace Nan;

class StreamEncodeWorker : public AsyncProgressQueueWorker<uint8_t> {
  public:
    StreamEncodeWorker(Callback *callback, StreamEncode* obj, BrotliEncoderOperation op);

    void Execute(const ExecutionProgress& progress);
    void HandleProgressCallback(const uint8_t *data, size_t size);

  private:
    ~StreamEncodeWorker();
    StreamEncode* obj;
    BrotliEncoderOperation op;
};

#endif
