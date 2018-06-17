#ifndef STREAM_CODER_H
#define STREAM_CODER_H

#include <vector>
#include <node_api.h>
#include "allocator.h"

class StreamCoder {
  public:
    Allocator alloc;
    std::vector<uint8_t*> pending_output;

    void ClearPendingOutput(napi_env env);
    void PendingChunksAsArray(napi_env env, napi_value* arr);
};

#endif
