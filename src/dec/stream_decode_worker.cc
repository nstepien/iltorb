#include "stream_decode_worker.h"

using namespace v8;

StreamDecodeWorker::StreamDecodeWorker(Nan::Callback *callback, StreamDecode* obj)
  : Nan::AsyncWorker(callback), obj(obj) {}

StreamDecodeWorker::~StreamDecodeWorker() {
}

void StreamDecodeWorker::Execute() {
  Allocator::AllocatedBuffer* buf_info;

  do {
    void* buf = obj->alloc.Alloc(16384);
    if (!buf) {
      res = BROTLI_RESULT_ERROR;
      return;
    }

    uint8_t* next_out = static_cast<uint8_t*>(buf);
    buf_info = Allocator::GetBufferInfo(buf);
    res = BrotliDecompressStream(&obj->available_in,
                                 &obj->next_in,
                                 &buf_info->available,
                                 &next_out,
                                 NULL,
                                 obj->state);

    obj->pending_output.push_back(static_cast<uint8_t*>(buf));
  } while(res == BROTLI_RESULT_NEEDS_MORE_OUTPUT);
}

void StreamDecodeWorker::HandleOKCallback() {
  if (res == BROTLI_RESULT_ERROR || res == BROTLI_RESULT_NEEDS_MORE_OUTPUT) {
    Local<Value> argv[] = {
      Nan::Error("Brotli failed to decompress.")
    };
    callback->Call(1, argv);
  } else {
    size_t n_chunks = obj->pending_output.size();
    Local<Array> chunks = Nan::New<Array>(n_chunks);

    for (size_t i = 0; i < n_chunks; i++) {
      uint8_t* current = obj->pending_output[i];
      Allocator::AllocatedBuffer* buf_info = Allocator::GetBufferInfo(current);
      Nan::Set(chunks, i, Nan::NewBuffer(reinterpret_cast<char*>(current),
                                         buf_info->size - buf_info->available,
                                         Allocator::NodeFree,
                                         NULL).ToLocalChecked());
    }
    obj->pending_output.clear();

    Local<Value> argv[] = {
      Nan::Null(),
      chunks
    };
    callback->Call(2, argv);
  }

  obj->alloc.ReportMemoryToV8();
}
