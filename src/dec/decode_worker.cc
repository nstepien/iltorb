#include "decode_worker.h"

using namespace v8;

DecodeWorker::DecodeWorker(Nan::Callback *callback, Local<Object> buffer)
  : Nan::AsyncWorker(callback) {
    input = BrotliInitMemInput(
      (const uint8_t*) node::Buffer::Data(buffer),
      node::Buffer::Length(buffer),
      &mem_input);
}

void DecodeWorker::Execute() {
  BrotliOutput output = BrotliInitBufferOutput(&mem_output);
  res = BrotliDecompress(input, output);
}

void DecodeWorker::HandleOKCallback() {
  if (res == BROTLI_RESULT_SUCCESS) {
    Local<Value> argv[] = {
      Nan::Null(),
      Nan::CopyBuffer(&mem_output[0], mem_output.length()).ToLocalChecked()
    };
    callback->Call(2, argv);
  } else {
    Local<Value> argv[] = {
      Nan::Error("Brotli failed to decompress.")
    };
    callback->Call(1, argv);
  }
}
