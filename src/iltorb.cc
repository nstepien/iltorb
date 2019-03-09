#include <nan.h>
#include "dec/stream_decode.h"
#include "enc/stream_encode.h"

using namespace v8;

#ifdef NODE_MODULE_INIT
NODE_MODULE_INIT() {
  StreamDecode::Init(exports);
  StreamEncode::Init(exports);
}
#else
NAN_MODULE_INIT(Init) {
  StreamDecode::Init(target);
  StreamEncode::Init(target);
}

NODE_MODULE(iltorb, Init)
#endif
