#include "get_params.h"

using namespace v8;
using namespace brotli;

BrotliParams getParams(Local<Object> userParams) {
  BrotliParams params;
  Local<String> key;

  key = Nan::New<String>("mode").ToLocalChecked();
  if (Nan::Has(userParams, key).FromJust()) {
    params.mode = (BrotliParams::Mode) Nan::Get(userParams, key).ToLocalChecked()->Int32Value();
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
