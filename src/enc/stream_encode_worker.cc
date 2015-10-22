#include "stream_encode_worker.h"

using namespace v8;
using namespace brotli;

StreamEncodeWorker::StreamEncodeWorker(Nan::Callback *callback, BrotliCompressor *compressor, bool is_last)
  : Nan::AsyncWorker(callback), compressor(compressor), is_last(is_last) {}

void StreamEncodeWorker::Execute() {
  uint8_t* buffer;
  res = compressor->WriteBrotliData(is_last, false, &output_size, &buffer);

  if (res && output_size > 0) {
    output_buffer = (char*) malloc(output_size);
    if (output_buffer == NULL) {
      res = false;
    } else {
      memcpy(output_buffer, buffer, output_size);
    }
  }
}

void StreamEncodeWorker::HandleOKCallback() {
  if (!res) {
    Local<Value> argv[] = {
      Nan::Error("Brotli failed to compress.")
    };
    callback->Call(1, argv);
  } else if (output_size > 0) {
    Local<Value> argv[] = {
      Nan::Null(),
      Nan::NewBuffer(output_buffer, output_size).ToLocalChecked()
    };
    callback->Call(2, argv);
  } else {
    Local<Value> argv[] = {
      Nan::Null()
    };
    callback->Call(1, argv);
  }
}
