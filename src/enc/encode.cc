#include <nan.h>
#include "buffer_out.h"
#include "encode_worker.h"
#include "get_params.h"
#include "stream_encode.h"
#include "../../brotli/enc/encode.h"

using namespace v8;
using namespace brotli;

NAN_METHOD(CompressAsync) {
  Local<Object> buffer = info[0]->ToObject();
  Nan::Callback *callback = new Nan::Callback(info[2].As<Function>());
  BrotliMemIn input(node::Buffer::Data(buffer), node::Buffer::Length(buffer));

  Nan::AsyncQueueWorker(new EncodeWorker(callback, getParams(info[1]->ToObject()), input));
}

NAN_METHOD(CompressSync) {
  Local<Object> buffer = info[0]->ToObject();
  BrotliMemIn input(node::Buffer::Data(buffer), node::Buffer::Length(buffer));
  BufferOut output;

  if (BrotliCompress(getParams(info[1]->ToObject()), &input, &output)) {
    info.GetReturnValue().Set(Nan::CopyBuffer(&output.buffer[0], output.buffer.length()).ToLocalChecked());
  } else {
    Nan::ThrowError("Brotli failed to compress.");
  }
}

NAN_MODULE_INIT(Init) {
  StreamEncode::Init(target);
  Nan::SetMethod(target, "compressAsync", CompressAsync);
  Nan::SetMethod(target, "compressSync", CompressSync);
}

NODE_MODULE(brotli, Init)
