#include "stream_encode_worker.h"

using namespace v8;

StreamEncodeWorker::StreamEncodeWorker(Nan::Callback *callback, StreamEncode* obj, BrotliEncoderOperation op)
  : Nan::AsyncWorker(callback), obj(obj), op(op) {}

StreamEncodeWorker::~StreamEncodeWorker() {
}

void StreamEncodeWorker::Execute() {
  do {
    size_t available_out = 0;
    bool res = BrotliEncoderCompressStream(obj->state,
                                           op,
                                           &obj->available_in,
                                           &obj->next_in,
                                           &available_out,
                                           NULL,
                                           NULL);

    if (res == BROTLI_FALSE) {
      return SetErrorMessage("Brotli failed to compress.");
    }

    while (BrotliEncoderHasMoreOutput(obj->state) == BROTLI_TRUE) {
      size_t size = 0;
      const uint8_t* output = BrotliEncoderTakeOutput(obj->state, &size);

      void* buf = obj->alloc.Alloc(size);
      if (!buf) {
        return SetErrorMessage("Brotli failed to compress.");
      }

      memcpy(buf, output, size);
      obj->pending_output.push_back(static_cast<uint8_t*>(buf));
    }
  } while (obj->available_in > 0);
}

void StreamEncodeWorker::HandleOKCallback() {
  Local<Value> argv[] = {
    Nan::Null(),
    obj->PendingChunksAsArray()
  };
  callback->Call(2, argv, async_resource);
  obj->alloc.ReportMemoryToV8();
}
