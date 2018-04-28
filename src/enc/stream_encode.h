#ifndef STREAM_ENCODE_H
#define STREAM_ENCODE_H

#include <nan.h>
#include "../common/allocator.h"
#include "brotli/encode.h"

using namespace v8;
using namespace Nan;

class StreamEncode : public Nan::ObjectWrap {
  public:
    static void Init(ADDON_REGISTER_FUNCTION_ARGS_TYPE target);

    Allocator alloc;

    const uint8_t* next_in;
    size_t available_in;

    Callback *progress;
    BrotliEncoderState* state;
  private:
    explicit StreamEncode(Local<Object> params, Callback *progress);
    ~StreamEncode();

    static NAN_METHOD(New);
    static NAN_METHOD(Transform);
    static NAN_METHOD(Flush);
    static Nan::Persistent<v8::Function> constructor;
};

#endif
