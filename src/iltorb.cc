#include <node_api.h>
#include "dec/stream_decode.h"
#include "enc/stream_encode.h"

NAPI_MODULE_INIT() {
  StreamDecode::Init(env, exports);
  StreamEncode::Init(env, exports);
  return exports;
}
