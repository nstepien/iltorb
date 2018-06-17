#ifndef ILTORB_ALLOCATOR_H
#define ILTORB_ALLOCATOR_H

#include <stdlib.h>
#include <node_api.h>

struct Allocator {
  Allocator() : allocated_unreported_memory(0) {}

  int64_t allocated_unreported_memory;

  struct AllocatedBuffer {
    size_t size;
    /* char data[...]; */
  };

  void* Alloc(size_t size);
  void Free(void* address);

  static AllocatedBuffer* GetBufferInfo(void* address);
  void ReportMemoryToV8(napi_env env);

  // Brotli-style parameter order.
  static void* Alloc(void* opaque, size_t size);
  static void Free(void* opaque, void* address) {
    Free(opaque, address, NULL);
  }
  static void Free(void* opaque, void* address, napi_env env);

  // Like Free, but in node::Buffer::FreeCallback style.
  static void NodeFree(napi_env env, void* address, void* opaque) {
    Free(opaque, address, env);
  }
};

#endif
