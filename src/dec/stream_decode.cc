#include "stream_decode.h"
#include "stream_decode_worker.h"

using namespace v8;

Nan::Persistent<Function> StreamDecode::constructor;

StreamDecode::StreamDecode() {
  BrotliStateInit(&state);
  output = BrotliInitBufferOutput(&mem_output);
}

StreamDecode::~StreamDecode() {
}

void StreamDecode::Init(Local<Object> target) {
  Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
  tpl->SetClassName(Nan::New<String>("StreamDecode").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  Nan::SetPrototypeMethod(tpl, "transform", Transform);
  Nan::SetPrototypeMethod(tpl, "flush", Flush);

  constructor.Reset(tpl->GetFunction());
  Nan::Set(target, Nan::New<String>("StreamDecode").ToLocalChecked(), tpl->GetFunction());
}

NAN_METHOD(StreamDecode::New) {
  StreamDecode* obj = new StreamDecode();
  obj->Wrap(info.This());
  info.GetReturnValue().Set(info.This());
}

NAN_METHOD(StreamDecode::Transform) {
  StreamDecode* obj = ObjectWrap::Unwrap<StreamDecode>(info.Holder());

  Local<Object> buffer = info[0]->ToObject();
  obj->input = BrotliInitMemInput(
    (const uint8_t*) node::Buffer::Data(buffer),
    node::Buffer::Length(buffer),
    &obj->mem_input);

  Nan::Callback *callback = new Nan::Callback(info[1].As<Function>());
  Nan::AsyncQueueWorker(new StreamDecodeWorker(callback, obj, false));
}

NAN_METHOD(StreamDecode::Flush) {
  StreamDecode* obj = ObjectWrap::Unwrap<StreamDecode>(info.Holder());

  Nan::Callback *callback = new Nan::Callback(info[0].As<Function>());
  Nan::AsyncQueueWorker(new StreamDecodeWorker(callback, obj, true));
}
