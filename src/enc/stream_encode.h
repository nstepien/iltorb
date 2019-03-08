#ifndef STREAM_ENCODE_H
#define STREAM_ENCODE_H

#include <nan.h>
#include "brotli/encode.h"
#include "../common/stream_coder.h"

class StreamEncode : public StreamCoder {
  public:
    static void Init(Nan::ADDON_REGISTER_FUNCTION_ARGS_TYPE target);

    bool isAsync;
    const uint8_t* next_in;
    size_t available_in;

    BrotliEncoderState* state;

  private:
    explicit StreamEncode(bool isAsyncParam, Local<Object> params);
    ~StreamEncode();

    static NAN_METHOD(New);
    static NAN_METHOD(Transform);
    static NAN_METHOD(Flush);
    static Nan::Persistent<v8::Function> constructor;
};

#endif
