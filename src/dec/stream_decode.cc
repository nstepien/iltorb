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

napi_value StreamDecode::Init(napi_env env, napi_value exports) {
  napi_value cons;
  napi_property_descriptor properties[] = {
    { "transform", NULL, Transform, NULL, NULL, NULL, napi_default, NULL },
    { "flush", NULL, Flush, NULL, NULL, NULL, napi_default, NULL }
  };

  napi_define_class(env, "StreamDecode", NAPI_AUTO_LENGTH, New, nullptr, 2, properties, &cons);
  napi_create_reference(env, cons, 1, &constructor);
  napi_set_named_property(env, exports, "StreamDecode", cons);

  return exports;
}

napi_value StreamDecode::New(napi_env env, napi_callback_info info) {
  size_t argc = 0;
  napi_value jsthis;
  napi_get_cb_info(env, info, &argc, nullptr, &jsthis, nullptr);

  StreamDecode* obj = new StreamDecode(env);

  obj->env_ = env;
  napi_wrap(env,
            jsthis,
            reinterpret_cast<void*>(obj),
            StreamDecode::Destructor,
            nullptr,
            &obj->wrapper_);

  return jsthis;
}

napi_value StreamDecode::Transform(napi_env env, napi_callback_info info) {
  size_t argc = 3;
  napi_value argv[3];
  napi_value jsthis;
  napi_get_cb_info(env, info, &argc, argv, &jsthis, nullptr);

  StreamDecode* obj;
  napi_unwrap(env, jsthis, reinterpret_cast<void**>(&obj));

  napi_get_buffer_info(env, argv[0], (void**)&obj->next_in, &obj->available_in);

  napi_create_reference(env, argv[1], 1, &obj->cb);

  bool isAsync;
  napi_get_value_bool(env, argv[2], &isAsync);

  if (isAsync) {
    napi_value resource_name;
    napi_create_string_utf8(env, "DecodeResource", NAPI_AUTO_LENGTH, &resource_name);

    napi_async_work work;
    napi_create_async_work(env,
                           nullptr,
                           resource_name,
                           ExecuteDecode,
                           CompleteDecode,
                           obj,
                           &work);

    napi_queue_async_work(env, work);
  } else {
    napi_status status;
    ExecuteDecode(env, obj);
    CompleteDecode(env, status, obj);
  }

  return nullptr;
}

napi_value StreamDecode::Flush(napi_env env, napi_callback_info info) {
  size_t argc = 2;
  napi_value argv[2];
  napi_value jsthis;
  napi_get_cb_info(env, info, &argc, argv, &jsthis, nullptr);

  StreamDecode* obj;
  napi_unwrap(env, jsthis, reinterpret_cast<void**>(&obj));

  napi_create_reference(env, argv[0], 1, &obj->cb);

  bool isAsync;
  napi_get_value_bool(env, argv[1], &isAsync);

  obj->next_in = nullptr;
  obj->available_in = 0;

  if (isAsync) {
    napi_value resource_name;
    napi_create_string_utf8(env, "DecodeResource", NAPI_AUTO_LENGTH, &resource_name);

    napi_async_work work;
    napi_create_async_work(env,
                           nullptr,
                           resource_name,
                           ExecuteDecode,
                           CompleteDecode,
                           obj,
                           &work);

    napi_queue_async_work(env, work);
  } else {
    napi_status status;
    ExecuteDecode(env, obj);
    CompleteDecode(env, status, obj);
  }

  return nullptr;
}
