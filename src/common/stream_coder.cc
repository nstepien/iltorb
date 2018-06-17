#include "stream_coder.h"

void StreamCoder::ClearPendingOutput(napi_env env) {
  size_t n_chunks = pending_output.size();
  for (size_t i = 0; i < n_chunks; i++) {
    alloc.Free(pending_output[i]);
  }

  alloc.ReportMemoryToV8(env);
}

void StreamCoder::PendingChunksAsArray(napi_env env, napi_value* arr) {
  size_t n_chunks = pending_output.size();

  napi_create_array_with_length(env, n_chunks, arr);

  for (size_t i = 0; i < n_chunks; i++) {
    uint8_t* chunk = pending_output[i];
    Allocator::AllocatedBuffer* buf_info = Allocator::GetBufferInfo(chunk);
    napi_value buf;
    napi_create_external_buffer(env, buf_info->size, chunk, Allocator::NodeFree, NULL, &buf);
    napi_set_element(env, *arr, i, buf);
  }

  pending_output.clear();
  alloc.ReportMemoryToV8(env);
}
