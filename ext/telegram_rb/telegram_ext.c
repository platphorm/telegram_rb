#include <ruby.h>

static VALUE rb_mTelegram;

static VALUE test_method(VALUE self){
  return rb_str_new2("hello world");
}

void Init_telegram_ext() {
  rb_mTelegram = rb_define_module("Telegram");

  rb_define_singleton_method(rb_mTelegram, "test_ext", test_method, 0);
}


