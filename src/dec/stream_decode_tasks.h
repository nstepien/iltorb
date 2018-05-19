#ifndef STREAM_DECODE_WORKER_H
#define STREAM_DECODE_WORKER_H

#include <string.h>
#include <node_api.h>
#include "brotli/decode.h"
#include "stream_decode.h"

void ExecuteDecode(napi_env env, void* data);
void CompleteDecode(napi_env env, napi_status, void* data);

#endif
