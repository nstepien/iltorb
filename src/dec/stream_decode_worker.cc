#include "stream_decode_worker.h"

StreamDecodeWorker::StreamDecodeWorker(Callback *callback, StreamDecode* obj)
  : AsyncProgressQueueWorker<uint8_t>(callback), obj(obj) {}

StreamDecodeWorker::~StreamDecodeWorker() {
}

void StreamDecodeWorker::Execute(const ExecutionProgress& progress) {
  BrotliDecoderResult res;
  do {
    size_t available_out = 0;
    res = BrotliDecoderDecompressStream(obj->state,
                                        &obj->available_in,
                                        &obj->next_in,
                                        &available_out,
                                        NULL,
                                        NULL);

    if (res == BROTLI_DECODER_RESULT_ERROR) {
      return SetErrorMessage("Brotli: failed to decompress");
    }

    while (BrotliDecoderHasMoreOutput(obj->state) == BROTLI_TRUE) {
      size_t size = 0;
      const uint8_t* output = BrotliDecoderTakeOutput(obj->state, &size);
      progress.Send(output, size);
    }
  } while(res == BROTLI_DECODER_RESULT_NEEDS_MORE_OUTPUT);
}

void StreamDecodeWorker::HandleProgressCallback(const uint8_t *data, size_t size) {
  Nan::HandleScope scope;

  Local<Value> argv[] = {
    CopyBuffer(reinterpret_cast<const char*>(data), size).ToLocalChecked()
  };
  obj->progress->Call(1, argv, async_resource);

  obj->alloc.ReportMemoryToV8();
}
