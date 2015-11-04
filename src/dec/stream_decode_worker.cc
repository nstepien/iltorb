#include "stream_decode_worker.h"

using namespace v8;

StreamDecodeWorker::StreamDecodeWorker(Nan::Callback *callback, StreamDecode* obj, bool finish)
  : Nan::AsyncWorker(callback), obj(obj), finish(finish) {}

StreamDecodeWorker::~StreamDecodeWorker() {
}

void StreamDecodeWorker::Execute() {
  res = BrotliDecompressStreaming(obj->input, obj->output, finish, &obj->state);
}

void StreamDecodeWorker::HandleOKCallback() {
  if (res == BROTLI_RESULT_ERROR || res == BROTLI_RESULT_NEEDS_MORE_OUTPUT) {
    Local<Value> argv[] = {
      Nan::Error("Brotli failed to decompress.")
    };
    callback->Call(1, argv);
  } else if (obj->mem_output.length() > obj->count_flushed) {
    Local<Value> argv[] = {
      Nan::Null(),
      Nan::CopyBuffer(
        &obj->mem_output[obj->count_flushed],
        obj->mem_output.length() - obj->count_flushed).ToLocalChecked()
    };
    obj->count_flushed = obj->mem_output.length();
    callback->Call(2, argv);
  } else {
    Local<Value> argv[] = {
      Nan::Null()
    };
    callback->Call(1, argv);
  }
}
