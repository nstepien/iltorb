#include <nan.h>
#include "../brotli/dec/decode.h"

using namespace v8;

int BrotliBufferOutputFunction(void* data, const uint8_t* buf, size_t count) {
  std::string *mem_output = (std::string*)data;
  mem_output->append((char*) buf, count);
  return (int)count;
}

BrotliOutput BrotliInitBufferOutput(std::string* mem_output) {
  BrotliOutput output;
  output.cb_ = &BrotliBufferOutputFunction;
  output.data_ = mem_output;
  return output;
}

class DecodeWorker : public Nan::AsyncWorker {
 public:
  DecodeWorker(Nan::Callback *callback, Local<Object> buffer)
    : Nan::AsyncWorker(callback) {
      input = BrotliInitMemInput(
        (const uint8_t*) node::Buffer::Data(buffer),
        node::Buffer::Length(buffer),
        &mem_input);
    }

  void Execute() {
    BrotliOutput output = BrotliInitBufferOutput(&mem_output);
    res = BrotliDecompress(input, output);
  }

  void HandleOKCallback() {
    if (res == 1) {
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

  private:
    int res;
    BrotliMemInput mem_input;
    BrotliInput input;
    std::string mem_output;
};

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
