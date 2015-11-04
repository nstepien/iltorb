#ifndef STREAM_ENCODE_H
#define STREAM_ENCODE_H

#include <nan.h>
#include "get_params.h"
#include "../../brotli/enc/encode.h"

class StreamEncode : public Nan::ObjectWrap {
  public:
    static void Init(v8::Local<v8::Object> target);

  private:
    explicit StreamEncode(brotli::BrotliParams params);
    ~StreamEncode();

    static NAN_METHOD(New);
    static NAN_METHOD(GetBlockSize);
    static NAN_METHOD(Copy);
    static NAN_METHOD(Encode);
    static inline Nan::Persistent<v8::Function> & constructor();
    brotli::BrotliCompressor *compressor;
};

#endif
