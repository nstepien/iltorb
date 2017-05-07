#include "stream_encode_worker.h"

using namespace v8;

StreamEncodeWorker::StreamEncodeWorker(Nan::Callback *callback, StreamEncode* obj, BrotliEncoderOperation op)
  : Nan::AsyncWorker(callback), obj(obj), op(op) {}

StreamEncodeWorker::~StreamEncodeWorker() {
}

void StreamEncodeWorker::Execute() {
  void* buf = obj->alloc.Alloc(131072);
  if (!buf) {
    res = BROTLI_FALSE;
    return;
  }

  uint8_t* next_out = static_cast<uint8_t*>(buf);
  Allocator::AllocatedBuffer* buf_info = Allocator::GetBufferInfo(buf);
  res = BrotliEncoderCompressStream(obj->state,
                                    op,
                                    &obj->available_in,
                                    &obj->next_in,
                                    &buf_info->available,
                                    &next_out,
                                    NULL);

  obj->pending_output.push_back(static_cast<uint8_t*>(buf));
}

void StreamEncodeWorker::HandleOKCallback() {
  if (res == BROTLI_FALSE) {
    Local<Value> argv[] = {
      Nan::Error("Brotli failed to compress.")
    };
    callback->Call(1, argv);
  } else {
    Local<Value> argv[] = {
      Nan::Null(),
      obj->PendingChunksAsArray()
    };
    callback->Call(2, argv);
  }

  obj->alloc.ReportMemoryToV8();
}
