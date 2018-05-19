#include "allocator.h"

void* Allocator::Alloc(void* opaque, size_t size) {
  return static_cast<Allocator*>(opaque)->Alloc(size);
}

void* Allocator::Alloc(size_t size) {
  void* realbuffer = malloc(size + sizeof(AllocatedBuffer));
  AllocatedBuffer* buf = static_cast<AllocatedBuffer*>(realbuffer);
  if (!buf) {
    return NULL;
  }

  buf->size = size;
  allocated_unreported_memory += size + sizeof(*buf);
  return static_cast<void*>(buf + 1);
}

Allocator::AllocatedBuffer* Allocator::GetBufferInfo(void* address) {
  return static_cast<AllocatedBuffer*>(address) - 1;
}

void Allocator::Free(void* opaque, void* address, napi_env env) {
  if (!address) {
    return;
  }

  AllocatedBuffer* buf = GetBufferInfo(address);
  int64_t size = buf->size + sizeof(*buf);

  if (opaque) {
    Allocator* alloc = static_cast<Allocator*>(opaque);
    alloc->allocated_unreported_memory -= size;
  } else {
    int64_t result;
    napi_adjust_external_memory(env, -size, &result);
  }

  free(buf);
}

void Allocator::Free(void* address) {
  Free(this, address, NULL);
}

void Allocator::ReportMemoryToV8(napi_env env) {
  int64_t result;
  napi_adjust_external_memory(env, allocated_unreported_memory, &result);
  allocated_unreported_memory = 0;
}
