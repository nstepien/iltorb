#ifndef STREAM_ENCODE_WORKER_H
#define STREAM_ENCODE_WORKER_H

#include <string.h>
#include <node_api.h>
#include "brotli/encode.h"
#include "stream_encode.h"

void StartEncode(napi_env env, StreamEncode* obj);
void ExecuteEncode(napi_env env, void* data);
void CompleteEncode(napi_env env, napi_status, void* data);

#endif
