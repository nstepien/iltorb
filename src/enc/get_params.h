#ifndef GET_PARAMS_H
#define GET_PARAMS_H

#include <nan.h>
#include "../../brotli/enc/encode.h"

brotli::BrotliParams getParams(v8::Local<v8::Object> userParams);

#endif
