#include <node_api.h>
#include "dec/stream_decode.h"
#include "enc/stream_encode.h"

#ifdef NAPI_MODULE_INIT
NAPI_MODULE_INIT() {
  StreamDecode::Init(env, exports);
  StreamEncode::Init(env, exports);
  return exports;
}
#else
napi_value Init(napi_env env, napi_value exports) {
  StreamDecode::Init(env, exports);
  StreamEncode::Init(env, exports);
  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)
#endif
