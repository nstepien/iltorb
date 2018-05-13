#include "stream_decode.h"

napi_ref StreamDecode::constructor;

StreamDecode::StreamDecode(napi_env env) {
  state = BrotliDecoderCreateInstance(Allocator::Alloc, Allocator::Free, &alloc);
  alloc.ReportMemoryToV8(env);
}

StreamDecode::~StreamDecode() {
  napi_delete_reference(env_, wrapper_);
  BrotliDecoderDestroyInstance(state);
}

void StreamDecode::Destructor(napi_env env, void* nativeObject, void* /*finalize_hint*/) {
  StreamDecode* obj = reinterpret_cast<StreamDecode*>(nativeObject);
  obj->~StreamDecode();
  obj->ClearPendingOutput(env);
}

#define DECLARE_NAPI_METHOD(name, func) \
  { name, 0, func, 0, 0, 0, napi_default, 0 }

napi_value StreamDecode::Init(napi_env env, napi_value exports) {
  napi_status status;
  napi_property_descriptor properties[] = {
    DECLARE_NAPI_METHOD("transform", Transform),
    DECLARE_NAPI_METHOD("flush", Flush)
  };

  napi_value cons;
  status = napi_define_class(env, "StreamDecode", NAPI_AUTO_LENGTH, New, nullptr, 2, properties, &cons);
  assert(status == napi_ok);

  status = napi_create_reference(env, cons, 1, &constructor);
  assert(status == napi_ok);

  status = napi_set_named_property(env, exports, "StreamDecode", cons);
  assert(status == napi_ok);

  return exports;
}

napi_value StreamDecode::New(napi_env env, napi_callback_info info) {
  napi_status status;

  size_t argc = 0;
  napi_value jsthis;
  status = napi_get_cb_info(env, info, &argc, nullptr, &jsthis, nullptr);
  assert(status == napi_ok);

  StreamDecode* obj = new StreamDecode(env);

  obj->env_ = env;
  status = napi_wrap(env,
                     jsthis,
                     reinterpret_cast<void*>(obj),
                     StreamDecode::Destructor,
                     nullptr,
                     &obj->wrapper_);
  assert(status == napi_ok);

  return jsthis;
}

napi_value StreamDecode::Transform(napi_env env, napi_callback_info info) {
  napi_status status;

  size_t argc = 3;
  napi_value argv[3];
  napi_value jsthis;
  status = napi_get_cb_info(env, info, &argc, argv, &jsthis, nullptr);
  assert(status == napi_ok);

  StreamDecode* obj;
  status = napi_unwrap(env, jsthis, reinterpret_cast<void**>(&obj));
  assert(status == napi_ok);

  status = napi_get_buffer_info(env, argv[0], (void**)&obj->next_in, &obj->available_in);
  assert(status == napi_ok);

  status = napi_create_reference(env, argv[1], 1, &obj->cb);
  assert(status == napi_ok);

  bool isAsync;
  status = napi_get_value_bool(env, argv[2], &isAsync);
  assert(status == napi_ok);

  if (isAsync) {
    napi_value resource_name;
    napi_create_string_utf8(env, "DecodeResource", NAPI_AUTO_LENGTH, &resource_name);

    napi_async_work work;
    status = napi_create_async_work(env,
                                    nullptr,
                                    resource_name,
                                    ExecuteDecode,
                                    CompleteDecode,
                                    obj,
                                    &work);
    assert(status == napi_ok);

    status = napi_queue_async_work(env, work);
    assert(status == napi_ok);
  } else {
    ExecuteDecode(env, obj);
    CompleteDecode(env, status, obj);
  }

  return nullptr;
}

napi_value StreamDecode::Flush(napi_env env, napi_callback_info info) {
  napi_status status;

  size_t argc = 2;
  napi_value argv[2];
  napi_value jsthis;
  status = napi_get_cb_info(env, info, &argc, argv, &jsthis, nullptr);
  assert(status == napi_ok);

  StreamDecode* obj;
  status = napi_unwrap(env, jsthis, reinterpret_cast<void**>(&obj));
  assert(status == napi_ok);

  status = napi_create_reference(env, argv[0], 1, &obj->cb);
  assert(status == napi_ok);

  bool isAsync;
  status = napi_get_value_bool(env, argv[1], &isAsync);
  assert(status == napi_ok);

  obj->next_in = nullptr;
  obj->available_in = 0;

  if (isAsync) {
    napi_value resource_name;
    napi_create_string_utf8(env, "DecodeResource", NAPI_AUTO_LENGTH, &resource_name);

    napi_async_work work;
    status = napi_create_async_work(env,
                                    nullptr,
                                    resource_name,
                                    ExecuteDecode,
                                    CompleteDecode,
                                    obj,
                                    &work);
    assert(status == napi_ok);

    status = napi_queue_async_work(env, work);
    assert(status == napi_ok);
  } else {
    ExecuteDecode(env, obj);
    CompleteDecode(env, status, obj);
  }

  return nullptr;
}
