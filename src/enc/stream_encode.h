#ifndef STREAM_ENCODE_H
#define STREAM_ENCODE_H

#include <node_api.h>
#include "brotli/encode.h"
#include "../common/stream_coder.h"
#include "stream_encode_tasks.h"

class StreamEncode : public StreamCoder {
  public:
    static napi_value Init(napi_env env, napi_value exports);
    static void Destructor(napi_env env, void* nativeObject, void* finalize_hint);

    napi_ref cb;
    BrotliEncoderOperation op;
    const uint8_t* next_in;
    size_t available_in;
    BrotliEncoderState* state;

  private:
    explicit StreamEncode(napi_env env, napi_value params);
    ~StreamEncode();
    void SetParameter(napi_env env, napi_value params, const char* key, BrotliEncoderParameter p);

    static napi_value New(napi_env env, napi_callback_info info);
    static napi_value Transform(napi_env env, napi_callback_info info);
    static napi_value Flush(napi_env env, napi_callback_info info);
    static napi_ref constructor;
    napi_env env_;
    napi_ref wrapper_;
};

#endif
