#include "encode_worker.h"

using namespace v8;
using namespace brotli;

EncodeWorker::EncodeWorker(Nan::Callback *callback, BrotliParams params, BrotliMemIn input)
  : Nan::AsyncWorker(callback), params(params), input(input) {}

void EncodeWorker::Execute() {
  res = BrotliCompress(params, &input, &output);
}

void EncodeWorker::HandleOKCallback() {
  if (res) {
    Local<Value> argv[] = {
    Nan::Null(),
    Nan::CopyBuffer(&output.buffer[0], output.buffer.length()).ToLocalChecked()
    };
    callback->Call(2, argv);
  } else {
    Local<Value> argv[] = {
    Nan::Error("Brotli failed to compress.")
    };
    callback->Call(1, argv);
  }
}
