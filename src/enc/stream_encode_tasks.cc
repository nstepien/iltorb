#include "stream_encode_tasks.h"

void StartEncode(napi_env env, StreamEncode* obj) {
  if (obj->isAsync) {
    napi_value resource_name;
    napi_create_string_utf8(env, "EncodeResource", NAPI_AUTO_LENGTH, &resource_name);

    napi_create_async_work(env,
                           nullptr,
                           resource_name,
                           ExecuteEncode,
                           CompleteEncode,
                           obj,
                           &obj->work);

    napi_queue_async_work(env, obj->work);
  } else {
    napi_status status = napi_ok;
    ExecuteEncode(env, obj);
    CompleteEncode(env, status, obj);
  }
}

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

void CompleteEncode(napi_env env, napi_status, void* data) {
  StreamEncode* obj = reinterpret_cast<StreamEncode*>(data);

  napi_value null;
  napi_get_null(env, &null);

  napi_value cb;
  napi_get_reference_value(env, obj->cbref, &cb);

  if (obj->work != NULL) {
    napi_delete_async_work(env, obj->work);
    obj->work = NULL;
  }
  if (obj->bufref != NULL) {
    napi_delete_reference(env, obj->bufref);
    obj->bufref = NULL;
  }
  napi_delete_reference(env, obj->cbref);

  if (obj->hasError) {
    napi_value msg;
    napi_create_string_utf8(env, "Brotli failed to compress.", NAPI_AUTO_LENGTH, &msg);

    napi_value err;
    napi_create_error(env, NULL, msg, &err);

    napi_value argv[] = {err};
    napi_call_function(env, null, cb, 1, argv, nullptr);
  } else {
    napi_value arr;
    obj->PendingChunksAsArray(env, &arr);

    napi_value argv[] = {null, arr};
    napi_call_function(env, null, cb, 2, argv, nullptr);
  }
}
