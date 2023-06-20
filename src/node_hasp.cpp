#include "node_hasp.h"
#include <cstring>

using namespace v8;

#define get_crypto_length(len) len > 16 ? len : 16

Persistent<Function> NodeHasp::constructor;

NodeHasp::NodeHasp() {
}

NodeHasp::~NodeHasp() {
}

void NodeHasp::Init(Local<Object> exports, Local<Context> context) {
  Isolate* isolate = Isolate::GetCurrent();

  Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, New);
  tpl->SetClassName(String::NewFromUtf8(isolate, "Hasp").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  NODE_SET_PROTOTYPE_METHOD(tpl, "login", login);
  NODE_SET_PROTOTYPE_METHOD(tpl, "logout", logout);
  NODE_SET_PROTOTYPE_METHOD(tpl, "getSize", get_size);
  NODE_SET_PROTOTYPE_METHOD(tpl, "write", write);
  NODE_SET_PROTOTYPE_METHOD(tpl, "read", read);

  constructor.Reset(isolate, tpl->GetFunction(context).ToLocalChecked());
  exports->Set(context, String::NewFromUtf8(isolate, "Hasp").ToLocalChecked(), tpl->GetFunction(context).ToLocalChecked());
}

void NodeHasp::New(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);

  if (args.IsConstructCall()) {
    NodeHasp* h = new NodeHasp();
    h->Wrap(args.This());
    args.GetReturnValue().Set(args.This());
  } else {
    const int argc = 1;
    Local<Value> argv[argc] = { args[0] };
    Local<Function> cons = Local<Function>::New(isolate, constructor);
    args.GetReturnValue().Set(cons->NewInstance(isolate->GetCurrentContext(), argc, argv).ToLocalChecked());
  }
}

void NodeHasp::login(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);

  if (args.Length() < 1) {
    isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, "Please specify vendor code").ToLocalChecked()));
    return;
  }

  if (!args[0]->IsString()) {
    isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, "Vendor code should be a string").ToLocalChecked()));
    return;
  }

  NodeHasp* h = ObjectWrap::Unwrap<NodeHasp>(args.Holder());
  String::Utf8Value vendor_code(isolate, args[0]);
  h->hasp.login(*vendor_code);
  if (h->hasp.is_error()) {
    isolate->ThrowException(Exception::Error(
      String::NewFromUtf8(isolate, h->hasp.get_message()).ToLocalChecked()
    ));
  }
  args.GetReturnValue().Set(Boolean::New(isolate, !h->hasp.is_error()));
}

void NodeHasp::logout(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);
  NodeHasp* h = ObjectWrap::Unwrap<NodeHasp>(args.Holder());
  h->hasp.logout();
  if (h->hasp.is_error()) {
    isolate->ThrowException(Exception::Error(
      String::NewFromUtf8(isolate, h->hasp.get_message()).ToLocalChecked()
    ));
  }
  args.GetReturnValue().Set(Boolean::New(isolate, !h->hasp.is_error()));
}

void NodeHasp::get_size(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = Isolate::GetCurrent();
  NodeHasp* h = ObjectWrap::Unwrap<NodeHasp>(args.Holder());
  int32_t fsize = static_cast<int32_t>(h->get_size(isolate));
  args.GetReturnValue().Set(Integer::New(isolate, fsize));
}

hasp_size_t NodeHasp::get_size(Isolate* isolate) {
  hasp_size_t fsize = hasp.get_size();
  if (hasp.is_error()) {
    isolate->ThrowException(Exception::Error(
      String::NewFromUtf8(isolate, hasp.get_message()).ToLocalChecked()
    ));
  }
  return fsize;
}

void NodeHasp::read(const FunctionCallbackInfo<Value>& args) {
  NodeHasp* h = ObjectWrap::Unwrap<NodeHasp>(args.Holder());
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);
  size_t length;
  char* content = h->hasp.read(length);
  if (h->hasp.is_error()) {
    isolate->ThrowException(Exception::Error(
      String::NewFromUtf8(isolate, h->hasp.get_message()).ToLocalChecked()
    ));
  }
  args.GetReturnValue().Set(String::NewFromUtf8(
    isolate, content, NewStringType::kNormal, length
  ).ToLocalChecked());
  delete content;
}

void NodeHasp::write(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);

  if (args.Length() < 1) {
    isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, "Wrong number of arguments").ToLocalChecked()));
    return;
  }

  if (!args[0]->IsString()) {
    isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, "First argument should be a string").ToLocalChecked()));
    return;
  }

  NodeHasp* h = ObjectWrap::Unwrap<NodeHasp>(args.Holder());
  String::Utf8Value input(isolate, args[0]);
  h->hasp.write(*input, (size_t) input.length());

  if (h->hasp.is_error()) {
    isolate->ThrowException(Exception::Error(
      String::NewFromUtf8(isolate, h->hasp.get_message()).ToLocalChecked()
    ));
  }
}
