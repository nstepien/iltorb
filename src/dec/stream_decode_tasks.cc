#include "stream_decode_tasks.h"

void ExecuteDecode(napi_env env, void* data) {
  StreamDecode* obj = reinterpret_cast<StreamDecode*>(data);
  BrotliDecoderResult res;

  do {
    size_t available_out = 0;
    res = BrotliDecoderDecompressStream(obj->state,
                                        &obj->available_in,
                                        &obj->next_in,
                                        &available_out,
                                        NULL,
                                        NULL);

    // if (res == BROTLI_DECODER_RESULT_ERROR) {
    //   return SetErrorMessage("Brotli failed to decompress.");
    // }

    while (BrotliDecoderHasMoreOutput(obj->state) == BROTLI_TRUE) {
      size_t size = 0;
      const uint8_t* output = BrotliDecoderTakeOutput(obj->state, &size);

      void* buf = obj->alloc.Alloc(size);
      // if (!buf) {
      //   return SetErrorMessage("Brotli failed to decompress.");
      // }

      memcpy(buf, output, size);
      obj->pending_output.push_back(static_cast<uint8_t*>(buf));
    }
  } while (res == BROTLI_DECODER_RESULT_NEEDS_MORE_OUTPUT);
}

void CompleteDecode(napi_env env, napi_status status, void* data) {
  StreamDecode* obj = reinterpret_cast<StreamDecode*>(data);

  napi_value null;
  status = napi_get_null(env, &null);
  assert(status == napi_ok);

  napi_value cb;
  status = napi_get_reference_value(env, obj->cb, &cb);
  assert(status == napi_ok);

  status = napi_delete_reference(env, obj->cb);
  assert(status == napi_ok);

  napi_value arr;
  obj->PendingChunksAsArray(env, &arr);

  napi_value argv[] = {null, arr};
  status = napi_call_function(env, null, cb, 2, argv, nullptr);
  assert(status == napi_ok);

  // obj->alloc.ReportMemoryToV8(env);
}
