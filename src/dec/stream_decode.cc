#include "stream_decode.h"
#include "stream_decode_tasks.h"

napi_ref StreamDecode::constructor;

StreamDecode::StreamDecode(napi_env env, napi_value async) {
  napi_get_value_bool(env, async, &isAsync);
  state = BrotliDecoderCreateInstance(Allocator::Alloc, Allocator::Free, &alloc);
  alloc.ReportMemoryToV8(env);
}

void StreamDecode::Destructor(napi_env env, void* nativeObject, void* /*finalize_hint*/) {
  StreamDecode* obj = reinterpret_cast<StreamDecode*>(nativeObject);
  BrotliDecoderDestroyInstance(obj->state);
  obj->ClearPendingOutput(env);
  delete obj;
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
  size_t argc = 1;
  napi_value argv[1];
  napi_value jsthis;
  napi_get_cb_info(env, info, &argc, argv, &jsthis, nullptr);

  StreamDecode* obj = new StreamDecode(env, argv[0]);

  napi_wrap(env,
            jsthis,
            reinterpret_cast<void*>(obj),
            StreamDecode::Destructor,
            nullptr,
            nullptr);

  return jsthis;
}

napi_value StreamDecode::Transform(napi_env env, napi_callback_info info) {
  size_t argc = 2;
  napi_value argv[2];
  napi_value jsthis;
  napi_get_cb_info(env, info, &argc, argv, &jsthis, nullptr);

  StreamDecode* obj;
  napi_unwrap(env, jsthis, reinterpret_cast<void**>(&obj));

  napi_get_buffer_info(env, argv[0], (void**)&obj->next_in, &obj->available_in);

  napi_create_reference(env, argv[0], 1, &obj->bufref);
  napi_create_reference(env, argv[1], 1, &obj->cbref);

  StartDecode(env, obj);

  return nullptr;
}

napi_value StreamDecode::Flush(napi_env env, napi_callback_info info) {
  size_t argc = 1;
  napi_value argv[1];
  napi_value jsthis;
  napi_get_cb_info(env, info, &argc, argv, &jsthis, nullptr);

  StreamDecode* obj;
  napi_unwrap(env, jsthis, reinterpret_cast<void**>(&obj));

  napi_create_reference(env, argv[0], 1, &obj->cbref);

  obj->next_in = nullptr;
  obj->available_in = 0;

  StartDecode(env, obj);

  return nullptr;
}
