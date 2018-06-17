#ifndef STREAM_ENCODE_H
#define STREAM_ENCODE_H

#include <node_api.h>
#include "brotli/encode.h"
#include "../common/stream_coder.h"

class StreamEncode : public StreamCoder {
  public:
    static napi_value Init(napi_env env, napi_value exports);
    static void Destructor(napi_env env, void* nativeObject, void* finalize_hint);

    bool isAsync = true;
    bool hasError = false;
    BrotliEncoderState* state;
    BrotliEncoderOperation op;
    const uint8_t* next_in;
    size_t available_in;
    napi_ref bufref = NULL;
    napi_ref cbref = NULL;
    napi_async_work work = NULL;

  private:
    explicit StreamEncode(napi_env env, napi_value async, napi_value params);
    void SetParameter(napi_env env, napi_value params, const char* key, BrotliEncoderParameter p);

    static napi_value New(napi_env env, napi_callback_info info);
    static napi_value Transform(napi_env env, napi_callback_info info);
    static napi_value Flush(napi_env env, napi_callback_info info);
    static napi_ref constructor;
};

#endif
