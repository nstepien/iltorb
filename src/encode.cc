#include <nan.h>
#include "../brotli/enc/encode.h"

using namespace v8;
using namespace brotli;

class EncodeWorker : public Nan::AsyncWorker {
 public:
  EncodeWorker(Nan::Callback *callback, BrotliParams params, Local<Object> buffer)
    : Nan::AsyncWorker(callback), params(params) {
      input_size = node::Buffer::Length(buffer);
      input_buffer = node::Buffer::Data(buffer);
    }

  void Execute() {
    uint8_t* buffer;

    BrotliCompressor compressor(params);
    compressor.CopyInputToRingBuffer(input_size, (uint8_t*) input_buffer);
    res = compressor.WriteBrotliData(true, false, &output_size, &buffer);

    if (res) {
      output_buffer = (char*) malloc(output_size);
      if (output_buffer == NULL) {
        res = false;
      } else {
        memcpy(output_buffer, buffer, output_size);
      }
    }
  }

  void HandleOKCallback() {
    if (res) {
      Local<Value> argv[] = {
        Nan::Null(),
        Nan::NewBuffer(output_buffer, output_size).ToLocalChecked()
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
    size_t input_size;
    char* input_buffer;
    size_t output_size = 0;
    char* output_buffer;
};

BrotliParams getParams(Local<Object> userParams) {
  BrotliParams params;
  Local<String> key;

  key = Nan::New<String>("mode").ToLocalChecked();
  if (Nan::Has(userParams, key).FromJust()) {
    params.mode = (BrotliParams::Mode) Nan::Get(userParams, key).ToLocalChecked()->Int32Value();
  }

  key = Nan::New<String>("quality").ToLocalChecked();
  if (Nan::Has(userParams, key).FromJust()) {
    params.quality = Nan::Get(userParams, key).ToLocalChecked()->Int32Value();
  }

  key = Nan::New<String>("lgwin").ToLocalChecked();
  if (Nan::Has(userParams, key).FromJust()) {
    params.lgwin = Nan::Get(userParams, key).ToLocalChecked()->Int32Value();
  }

  key = Nan::New<String>("lgblock").ToLocalChecked();
  if (Nan::Has(userParams, key).FromJust()) {
    params.lgblock = Nan::Get(userParams, key).ToLocalChecked()->Int32Value();
  }

  return params;
}

NAN_METHOD(CompressAsync) {
  Local<Object> buffer = info[0]->ToObject();
  Nan::Callback *callback = new Nan::Callback(info[2].As<Function>());
  Nan::AsyncQueueWorker(new EncodeWorker(callback, getParams(info[1]->ToObject()), buffer));
}

NAN_METHOD(CompressSync) {
  size_t output_size = 0;
  uint8_t* output_buffer;
  Local<Object> buffer = info[0]->ToObject();

  BrotliCompressor compressor(getParams(info[1]->ToObject()));
  compressor.CopyInputToRingBuffer(node::Buffer::Length(buffer), (uint8_t*) node::Buffer::Data(buffer));

  if (compressor.WriteBrotliData(true, false, &output_size, &output_buffer)) {
    info.GetReturnValue().Set(Nan::CopyBuffer((char*) output_buffer, output_size).ToLocalChecked());
  } else {
    Nan::ThrowError("Brotli failed to compress.");
  }
}

NAN_MODULE_INIT(Init) {
  Nan::SetMethod(target, "compressAsync", CompressAsync);
  Nan::SetMethod(target, "compressSync", CompressSync);
}

NODE_MODULE(brotli, Init)
