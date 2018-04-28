#include "stream_encode_worker.h"

StreamEncodeWorker::StreamEncodeWorker(Callback *callback, StreamEncode* obj, BrotliEncoderOperation op)
  : AsyncProgressQueueWorker<uint8_t>(callback), obj(obj), op(op) {}

StreamEncodeWorker::~StreamEncodeWorker() {
}

void StreamEncodeWorker::Execute(const ExecutionProgress& progress) {
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
      return SetErrorMessage("Brotli: failed to compress");
    }

    if (BrotliEncoderHasMoreOutput(obj->state) == BROTLI_TRUE) {
      size_t size = 0;
      const uint8_t* output = BrotliEncoderTakeOutput(obj->state, &size);
      progress.Send(output, size);
    }
  } while (obj->available_in > 0);
}

void StreamEncodeWorker::HandleProgressCallback(const uint8_t *data, size_t size) {
  Nan::HandleScope scope;

  Local<Value> argv[] = {
    CopyBuffer(reinterpret_cast<const char*>(data), size).ToLocalChecked()
  };
  obj->progress->Call(1, argv, this->async_resource);

  obj->alloc.ReportMemoryToV8();
}
