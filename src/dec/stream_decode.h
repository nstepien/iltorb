#ifndef STREAM_DECODE_H
#define STREAM_DECODE_H

#include <nan.h>
#include "../common/allocator.h"
#include "brotli/decode.h"

using namespace v8;
using namespace Nan;

class StreamDecode : public Nan::ObjectWrap {
  public:
    static void Init(ADDON_REGISTER_FUNCTION_ARGS_TYPE target);

    Allocator alloc;

    const uint8_t* next_in;
    size_t available_in;

    Callback *progress;
    BrotliDecoderState* state;
  private:
    explicit StreamDecode(Callback *progress);
    ~StreamDecode();

    static NAN_METHOD(New);
    static NAN_METHOD(Transform);
    static NAN_METHOD(Flush);
    static Nan::Persistent<v8::Function> constructor;
};

#endif
