#include "stream_decode.h"
#include "stream_decode_worker.h"

StreamDecode::StreamDecode(bool async, Callback *progress) : async(async), progress(progress) {
  state = BrotliDecoderCreateInstance(Allocator::Alloc, Allocator::Free, &alloc);
  alloc.ReportMemoryToV8();
}

StreamDecode::~StreamDecode() {
  BrotliDecoderDestroyInstance(state);
  delete progress;
  alloc.ReportMemoryToV8();
}

void StreamDecode::Init(ADDON_REGISTER_FUNCTION_ARGS_TYPE target) {
  Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
  tpl->SetClassName(Nan::New("StreamDecode").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  Nan::SetPrototypeMethod(tpl, "transform", Transform);
  Nan::SetPrototypeMethod(tpl, "flush", Flush);

  constructor.Reset(Nan::GetFunction(tpl).ToLocalChecked());
  Nan::Set(target, Nan::New("StreamDecode").ToLocalChecked(),
    Nan::GetFunction(tpl).ToLocalChecked());
}

NAN_METHOD(StreamDecode::New) {
  Nan::Callback *progress = new Nan::Callback(info[1].As<Function>());
  StreamDecode* obj = new StreamDecode(info[0]->BooleanValue(), progress);
  obj->Wrap(info.This());
  info.GetReturnValue().Set(info.This());
}

NAN_METHOD(StreamDecode::Transform) {
  StreamDecode* obj = ObjectWrap::Unwrap<StreamDecode>(info.Holder());

  Local<Object> buffer = info[0]->ToObject();
  obj->next_in = (const uint8_t*) node::Buffer::Data(buffer);
  obj->available_in = node::Buffer::Length(buffer);

  Nan::Callback *callback = new Nan::Callback(info[1].As<Function>());
  AsyncWorker *worker = new StreamDecodeWorker(callback, obj);
  if (obj->async) {
    Nan::AsyncQueueWorker(worker);
  } else {
    worker->Execute();
    worker->WorkComplete();
    worker->Destroy();
  }
}

NAN_METHOD(StreamDecode::Flush) {
  StreamDecode* obj = ObjectWrap::Unwrap<StreamDecode>(info.Holder());

  Nan::Callback *callback = new Nan::Callback(info[0].As<Function>());
  obj->next_in = nullptr;
  obj->available_in = 0;
  AsyncWorker *worker = new StreamDecodeWorker(callback, obj);
  if (obj->async) {
    Nan::AsyncQueueWorker(worker);
  } else {
    worker->Execute();
    worker->WorkComplete();
    worker->Destroy();
  }
}

Nan::Persistent<Function> StreamDecode::constructor;
