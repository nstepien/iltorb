#include "stream_encode.h"
#include "stream_encode_worker.h"

using namespace v8;
using namespace brotli;

StreamEncode::StreamEncode(BrotliParams params) {
  compressor = new BrotliCompressor(params);
}

StreamEncode::~StreamEncode() {
  constructor.Reset();
  delete compressor;
}

void StreamEncode::Init(Local<Object> target) {
  Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
  tpl->SetClassName(Nan::New("StreamEncode").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  Nan::SetPrototypeMethod(tpl, "getBlockSize", GetBlockSize);
  Nan::SetPrototypeMethod(tpl, "copy", Copy);
  Nan::SetPrototypeMethod(tpl, "encode", Encode);

  constructor.Reset(Nan::GetFunction(tpl).ToLocalChecked());
  Nan::Set(target, Nan::New("StreamEncode").ToLocalChecked(),
    Nan::GetFunction(tpl).ToLocalChecked());
}

NAN_METHOD(StreamEncode::New) {
  StreamEncode* obj = new StreamEncode(getParams(info[0]->ToObject()));
  obj->Wrap(info.This());
  info.GetReturnValue().Set(info.This());
}

NAN_METHOD(StreamEncode::GetBlockSize) {
  StreamEncode* obj = ObjectWrap::Unwrap<StreamEncode>(info.Holder());
  info.GetReturnValue().Set(Nan::New<Number>(obj->compressor->input_block_size()));
}

NAN_METHOD(StreamEncode::Copy) {
  StreamEncode* obj = ObjectWrap::Unwrap<StreamEncode>(info.Holder());

  Local<Object> buffer = info[0]->ToObject();
  const size_t input_size = node::Buffer::Length(buffer);
  const char* input_buffer = node::Buffer::Data(buffer);

  obj->compressor->CopyInputToRingBuffer(input_size, (uint8_t*) input_buffer);
}

NAN_METHOD(StreamEncode::Encode) {
  StreamEncode* obj = ObjectWrap::Unwrap<StreamEncode>(info.Holder());

  bool is_last = info[0]->BooleanValue();
  Nan::Callback *callback = new Nan::Callback(info[1].As<Function>());
  Nan::AsyncQueueWorker(new StreamEncodeWorker(callback, obj->compressor, is_last));
}

Nan::Persistent<Function> StreamEncode::constructor;
