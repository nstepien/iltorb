#include "get_params.h"

using namespace v8;

BrotliEncoderParams getParams(Local<Object> userParams) {
  BrotliEncoderParams params = {
    BROTLI_DEFAULT_MODE,
    BROTLI_DEFAULT_QUALITY,
    BROTLI_DEFAULT_WINDOW,
    0
  };
  Local<String> key;

  key = Nan::New<String>("mode").ToLocalChecked();
  if (Nan::Has(userParams, key).FromJust()) {
    params.mode = (BrotliEncoderMode) Nan::Get(userParams, key).ToLocalChecked()->Int32Value();
  }

  key = Nan::New<String>("quality").ToLocalChecked();
  if (Nan::Has(userParams, key).FromJust()) {
    params.quality = Nan::Get(userParams, key).ToLocalChecked()->Int32Value();
  }

  key = Nan::New<String>("lgwin").ToLocalChecked();
  if (Nan::Has(userParams, key).FromJust()) {
    params.lgwin = Nan::Get(userParams, key).ToLocalChecked()->Int32Value();
  }

  key = Nan::New<String>("lgblock").ToLocalChecked();
  if (Nan::Has(userParams, key).FromJust()) {
    params.lgblock = Nan::Get(userParams, key).ToLocalChecked()->Int32Value();
  }

  return params;
}
