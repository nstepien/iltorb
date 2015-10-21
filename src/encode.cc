#include <nan.h>
#include "stream_encode.h"
#include "get_params.h"
#include "../brotli/enc/encode.h"

using namespace v8;
using namespace brotli;

class BufferOut : public BrotliOut {
  public:
    bool Write(const void* buf, size_t n) {
      buffer.append((char*) buf, n);
      return true;
    }

    std::string buffer;
};

class EncodeWorker : public Nan::AsyncWorker {
 public:
  EncodeWorker(Nan::Callback *callback, BrotliParams params, BrotliMemIn input)
    : Nan::AsyncWorker(callback), params(params), input(input) {}

  void Execute() {
    res = BrotliCompress(params, &input, &output);
  }

  void HandleOKCallback() {
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

  private:
    bool res;
    BrotliParams params;
    BrotliMemIn input;
    BufferOut output;
};

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
