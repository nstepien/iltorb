#include <nan.h>
#include "buffer_output.h"
#include "decode_worker.h"
#include "../../brotli/dec/decode.h"

using namespace v8;

NAN_METHOD(DecompressAsync) {
  Nan::Callback *callback = new Nan::Callback(info[1].As<Function>());
  Nan::AsyncQueueWorker(new DecodeWorker(callback, info[0]->ToObject()));
}

NAN_METHOD(DecompressSync) {
  Local<Object> buffer = info[0]->ToObject();

  BrotliMemInput mem_input;
  BrotliInput input = BrotliInitMemInput(
    (const uint8_t*) node::Buffer::Data(buffer),
    node::Buffer::Length(buffer),
    &mem_input);
  std::string mem_output;
  BrotliOutput output = BrotliInitBufferOutput(&mem_output);

  if (BrotliDecompress(input, output) == 1) {
    info.GetReturnValue().Set(Nan::CopyBuffer(&mem_output[0], mem_output.length()).ToLocalChecked());
  } else {
    Nan::ThrowError("Brotli failed to decompress.");
  }
}

NAN_MODULE_INIT(Init) {
  Nan::SetMethod(target, "decompressAsync", DecompressAsync);
  Nan::SetMethod(target, "decompressSync", DecompressSync);
}

NODE_MODULE(brotli, Init)
