#include "stream_encode.h"

napi_ref StreamEncode::constructor;

StreamEncode::StreamEncode(napi_env env, napi_value params) {
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

  // alloc.ReportMemoryToV8();
}

StreamEncode::~StreamEncode() {
  napi_delete_reference(env_, wrapper_);
  BrotliEncoderDestroyInstance(state);
}

void StreamEncode::Destructor(napi_env env, void* nativeObject, void* /*finalize_hint*/) {
  reinterpret_cast<StreamEncode*>(nativeObject)->~StreamEncode();
}

void StreamEncode::SetParameter(napi_env env, napi_value params, const char* key, BrotliEncoderParameter p) {
  napi_status status;

  bool hasProp;
  status = napi_has_named_property(env, params, key, &hasProp);
  assert(status == napi_ok);

  if (!hasProp) {
    return;
  }

  napi_value prop;
  status = napi_get_named_property(env, params, key, &prop);
  assert(status == napi_ok);

  napi_valuetype valuetype;
  status = napi_typeof(env, prop, &valuetype);
  assert(status == napi_ok);

  if (valuetype != napi_number) {
    status = napi_coerce_to_number(env, prop, &prop);
    assert(status == napi_ok);
  }

  uint32_t value;
  status = napi_get_value_uint32(env, prop, &value);
  assert(status == napi_ok);

  BrotliEncoderSetParameter(state, p, value);
}

#define DECLARE_NAPI_METHOD(name, func) \
  { name, 0, func, 0, 0, 0, napi_default, 0 }

napi_value StreamEncode::Init(napi_env env, napi_value exports) {
  napi_status status;
  napi_property_descriptor properties[] = {
    DECLARE_NAPI_METHOD("transform", Transform),
    DECLARE_NAPI_METHOD("flush", Flush)
  };

  napi_value cons;
  status = napi_define_class(env, "StreamEncode", NAPI_AUTO_LENGTH, New, nullptr, 2, properties, &cons);
  assert(status == napi_ok);

  status = napi_create_reference(env, cons, 1, &constructor);
  assert(status == napi_ok);

  status = napi_set_named_property(env, exports, "StreamEncode", cons);
  assert(status == napi_ok);

  return exports;
}

napi_value StreamEncode::New(napi_env env, napi_callback_info info) {
  napi_status status;

  size_t argc = 1;
  napi_value argv[1];
  napi_value jsthis;
  status = napi_get_cb_info(env, info, &argc, argv, &jsthis, nullptr);
  assert(status == napi_ok);

  StreamEncode* obj = new StreamEncode(env, argv[0]);

  obj->env_ = env;
  status = napi_wrap(env,
                     jsthis,
                     reinterpret_cast<void*>(obj),
                     StreamEncode::Destructor,
                     nullptr,
                     &obj->wrapper_);
  assert(status == napi_ok);

  return jsthis;
}

napi_value StreamEncode::Transform(napi_env env, napi_callback_info info) {
  napi_status status;

  size_t argc = 3;
  napi_value argv[3];
  napi_value jsthis;
  status = napi_get_cb_info(env, info, &argc, argv, &jsthis, nullptr);
  assert(status == napi_ok);

  StreamEncode* obj;
  status = napi_unwrap(env, jsthis, reinterpret_cast<void**>(&obj));
  assert(status == napi_ok);

  status = napi_get_buffer_info(env, argv[0], (void**)&obj->next_in, &obj->available_in);
  assert(status == napi_ok);

  status = napi_create_reference(env, argv[1], 1, &obj->cb);
  assert(status == napi_ok);

  bool isAsync;
  status = napi_get_value_bool(env, argv[2], &isAsync);
  assert(status == napi_ok);

  obj->op = BROTLI_OPERATION_PROCESS;

  if (isAsync) {
    napi_value resource_name;
    napi_create_string_utf8(env, "EncodeResource", NAPI_AUTO_LENGTH, &resource_name);

    napi_async_work work;
    status = napi_create_async_work(env,
                                    nullptr,
                                    resource_name,
                                    ExecuteEncode,
                                    CompleteEncode,
                                    obj,
                                    &work);
    assert(status == napi_ok);

    status = napi_queue_async_work(env, work);
    assert(status == napi_ok);
  } else {
    ExecuteEncode(env, obj);
    CompleteEncode(env, status, obj);
  }

  return nullptr;
}

napi_value StreamEncode::Flush(napi_env env, napi_callback_info info) {
  napi_status status;

  size_t argc = 3;
  napi_value argv[3];
  napi_value jsthis;
  status = napi_get_cb_info(env, info, &argc, argv, &jsthis, nullptr);
  assert(status == napi_ok);

  StreamEncode* obj;
  status = napi_unwrap(env, jsthis, reinterpret_cast<void**>(&obj));
  assert(status == napi_ok);

  bool isFinish;
  status = napi_get_value_bool(env, argv[0], &isFinish);
  assert(status == napi_ok);

  status = napi_create_reference(env, argv[1], 1, &obj->cb);
  assert(status == napi_ok);

  bool isAsync;
  status = napi_get_value_bool(env, argv[2], &isAsync);
  assert(status == napi_ok);

  obj->op = isFinish
    ? BROTLI_OPERATION_FINISH
    : BROTLI_OPERATION_FLUSH;

  obj->next_in = nullptr;
  obj->available_in = 0;

  if (isAsync) {
    napi_value resource_name;
    napi_create_string_utf8(env, "EncodeResource", NAPI_AUTO_LENGTH, &resource_name);

    napi_async_work work;
    status = napi_create_async_work(env,
                                    nullptr,
                                    resource_name,
                                    ExecuteEncode,
                                    CompleteEncode,
                                    obj,
                                    &work);
    assert(status == napi_ok);

    status = napi_queue_async_work(env, work);
    assert(status == napi_ok);
  } else {
    ExecuteEncode(env, obj);
    CompleteEncode(env, status, obj);
  }

  return nullptr;
}
