#include "stream_encode.h"
#include "stream_encode_tasks.h"

napi_ref StreamEncode::constructor;

StreamEncode::StreamEncode(napi_env env, napi_value async, napi_value params) {
  napi_get_value_bool(env, async, &isAsync);
  state = BrotliEncoderCreateInstance(Allocator::Alloc, Allocator::Free, &alloc);

  SetParameter(env, params, "mode", BROTLI_PARAM_MODE);
  SetParameter(env, params, "quality", BROTLI_PARAM_QUALITY);
  SetParameter(env, params, "lgwin", BROTLI_PARAM_LGWIN);
  SetParameter(env, params, "lgblock", BROTLI_PARAM_LGBLOCK);
  SetParameter(env, params, "disable_literal_context_modeling", BROTLI_PARAM_DISABLE_LITERAL_CONTEXT_MODELING);
  SetParameter(env, params, "size_hint", BROTLI_PARAM_SIZE_HINT);
  SetParameter(env, params, "large_window", BROTLI_PARAM_LARGE_WINDOW);
  SetParameter(env, params, "npostfix", BROTLI_PARAM_NPOSTFIX);
  SetParameter(env, params, "ndirect", BROTLI_PARAM_NDIRECT);

  alloc.ReportMemoryToV8(env);
}

void StreamEncode::Destructor(napi_env env, void* nativeObject, void* /*finalize_hint*/) {
  StreamEncode* obj = reinterpret_cast<StreamEncode*>(nativeObject);
  BrotliEncoderDestroyInstance(obj->state);
  obj->ClearPendingOutput(env);
  delete obj;
}

void StreamEncode::SetParameter(napi_env env, napi_value params, const char* key, BrotliEncoderParameter p) {
  bool hasProp;
  napi_has_named_property(env, params, key, &hasProp);

  if (!hasProp) {
    return;
  }

  napi_value prop;
  napi_get_named_property(env, params, key, &prop);

  napi_valuetype valuetype;
  napi_typeof(env, prop, &valuetype);

  if (valuetype == napi_boolean) {
    napi_coerce_to_number(env, prop, &prop);
  } else if (valuetype != napi_number) {
    return;
  }

  uint32_t value;
  napi_get_value_uint32(env, prop, &value);

  BrotliEncoderSetParameter(state, p, value);
}

napi_value StreamEncode::Init(napi_env env, napi_value exports) {
  napi_value cons;
  napi_property_descriptor properties[] = {
    { "transform", NULL, Transform, NULL, NULL, NULL, napi_default, NULL },
    { "flush", NULL, Flush, NULL, NULL, NULL, napi_default, NULL }
  };

  napi_define_class(env, "StreamEncode", NAPI_AUTO_LENGTH, New, nullptr, 2, properties, &cons);
  napi_create_reference(env, cons, 1, &constructor);
  napi_set_named_property(env, exports, "StreamEncode", cons);

  return exports;
}

napi_value StreamEncode::New(napi_env env, napi_callback_info info) {
  size_t argc = 2;
  napi_value argv[2];
  napi_value jsthis;
  napi_get_cb_info(env, info, &argc, argv, &jsthis, nullptr);

  StreamEncode* obj = new StreamEncode(env, argv[0], argv[1]);

  napi_wrap(env,
            jsthis,
            reinterpret_cast<void*>(obj),
            StreamEncode::Destructor,
            nullptr,
            nullptr);

  return jsthis;
}

napi_value StreamEncode::Transform(napi_env env, napi_callback_info info) {
  size_t argc = 2;
  napi_value argv[2];
  napi_value jsthis;
  napi_get_cb_info(env, info, &argc, argv, &jsthis, nullptr);

  StreamEncode* obj;
  napi_unwrap(env, jsthis, reinterpret_cast<void**>(&obj));

  napi_get_buffer_info(env, argv[0], (void**)&obj->next_in, &obj->available_in);

  napi_create_reference(env, argv[0], 1, &obj->bufref);
  napi_create_reference(env, argv[1], 1, &obj->cbref);

  obj->op = BROTLI_OPERATION_PROCESS;

  StartEncode(env, obj);

  return nullptr;
}

napi_value StreamEncode::Flush(napi_env env, napi_callback_info info) {
  size_t argc = 2;
  napi_value argv[2];
  napi_value jsthis;
  napi_get_cb_info(env, info, &argc, argv, &jsthis, nullptr);

  StreamEncode* obj;
  napi_unwrap(env, jsthis, reinterpret_cast<void**>(&obj));

  bool isFinish;
  napi_get_value_bool(env, argv[0], &isFinish);

  napi_create_reference(env, argv[1], 1, &obj->cbref);

  obj->op = isFinish
    ? BROTLI_OPERATION_FINISH
    : BROTLI_OPERATION_FLUSH;

  obj->next_in = nullptr;
  obj->available_in = 0;

  StartEncode(env, obj);

  return nullptr;
}
