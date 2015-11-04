#ifndef STREAM_DECODE_H
#define STREAM_DECODE_H

#include <nan.h>
#include "buffer_output.h"
#include "../../brotli/dec/decode.h"

class StreamDecode : public Nan::ObjectWrap {
  public:
    static void Init(v8::Local<v8::Object> target);

    BrotliInput input;
    BrotliOutput output;
    std::string mem_output;
    BrotliState state;
    size_t count_flushed = 0;

  private:
    explicit StreamDecode();
    ~StreamDecode();

    static NAN_METHOD(New);
    static NAN_METHOD(Transform);
    static NAN_METHOD(Flush);
    static inline Nan::Persistent<v8::Function> & constructor();
    BrotliMemInput mem_input;
};

#endif
