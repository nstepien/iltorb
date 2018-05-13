#include "stream_encode_tasks.h"

void ExecuteEncode(napi_env env, void* data) {
  StreamEncode* obj = reinterpret_cast<StreamEncode*>(data);

  do {
    size_t available_out = 0;
    bool res = BrotliEncoderCompressStream(obj->state,
                                           obj->op,
                                           &obj->available_in,
                                           &obj->next_in,
                                           &available_out,
                                           NULL,
                                           NULL);

    if (res == BROTLI_FALSE) {
      obj->hasError = true;
      return;
    }

    while (BrotliEncoderHasMoreOutput(obj->state) == BROTLI_TRUE) {
      size_t size = 0;
      const uint8_t* output = BrotliEncoderTakeOutput(obj->state, &size);

      void* buf = obj->alloc.Alloc(size);
      if (!buf) {
        obj->hasError = true;
        return;
      }

      memcpy(buf, output, size);
      obj->pending_output.push_back(static_cast<uint8_t*>(buf));
    }
  } while (obj->available_in > 0);
}

void CompleteEncode(napi_env env, napi_status status, void* data) {
  StreamEncode* obj = reinterpret_cast<StreamEncode*>(data);

  napi_value null;
  status = napi_get_null(env, &null);
  assert(status == napi_ok);

  napi_value cb;
  status = napi_get_reference_value(env, obj->cb, &cb);
  assert(status == napi_ok);

  status = napi_delete_reference(env, obj->cb);
  assert(status == napi_ok);

  if (obj->hasError) {
    napi_value msg;
    status = napi_create_string_utf8(env, "Brotli failed to compress.", NAPI_AUTO_LENGTH, &msg);
    assert(status == napi_ok);

    napi_value err;
    status = napi_create_error(env, NULL, msg, &err);
    assert(status == napi_ok);

    napi_value argv[] = {err};
    status = napi_call_function(env, null, cb, 1, argv, nullptr);
    assert(status == napi_ok || status == napi_pending_exception);
  } else {
    napi_value arr;
    obj->PendingChunksAsArray(env, &arr);

    napi_value argv[] = {null, arr};
    status = napi_call_function(env, null, cb, 2, argv, nullptr);
    assert(status == napi_ok);
  }

  obj->alloc.ReportMemoryToV8(env);
}
