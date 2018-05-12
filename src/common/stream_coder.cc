#include "stream_coder.h"

StreamCoder::StreamCoder() {
}

StreamCoder::~StreamCoder() {
  size_t n_chunks = pending_output.size();
  for (size_t i = 0; i < n_chunks; i++) {
    alloc.Free(pending_output[i]);
  }

  // alloc.ReportMemoryToV8();
}

void StreamCoder::PendingChunksAsArray(napi_env env, napi_value* arr) {
  napi_status status;
  size_t n_chunks = pending_output.size();

  status = napi_create_array_with_length(env, n_chunks, arr);
  assert(status == napi_ok);

  for (size_t i = 0; i < n_chunks; i++) {
    uint8_t* chunk = pending_output[i];
    Allocator::AllocatedBuffer* buf_info = Allocator::GetBufferInfo(chunk);
    napi_value buf;
    status = napi_create_external_buffer(env, buf_info->size, chunk, Allocator::NodeFree, NULL, &buf);
    assert(status == napi_ok);
    status = napi_set_element(env, *arr, i, buf);
    assert(status == napi_ok);
  }
  pending_output.clear();
}
