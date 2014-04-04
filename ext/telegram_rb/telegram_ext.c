#include <ruby.h>
#include "main.h"

static VALUE rb_mTelegram;
static VALUE rb_cPeerId;
static VALUE rb_cMessage;
static VALUE rb_cUser;
extern peer_t *Peers[];
extern int peer_num;

static VALUE load_config(VALUE self, VALUE pub_key){
  Check_Type(pub_key, T_STRING);

  telegram_main(RSTRING_PTR(pub_key));

  return Qnil;
}

VALUE build_peer_rb_obj(peer_id_t peer){
  VALUE argv[2];

  argv[0] = INT2FIX(peer.type);
  argv[1] = INT2FIX(peer.id);

  return rb_class_new_instance(2, argv, rb_cPeerId);
}

void tel_new_msg(struct message *M, int fn){
  VALUE argv[0];
  VALUE msg = rb_class_new_instance(0, argv, rb_cMessage);

  rb_iv_set(msg, "@id", LONG2NUM(M->id));
  rb_iv_set(msg, "@flags", INT2FIX(M->flags));
  rb_iv_set(msg, "@date", LONG2NUM(M->date));
  rb_iv_set(msg, "@text", rb_str_new2(M->message));
  rb_iv_set(msg, "@from_id", build_peer_rb_obj(M->from_id));
  rb_iv_set(msg, "@to_id", build_peer_rb_obj(M->to_id));

  //printf("Funtion number: %d", fn);

  ID sym_rev_message = rb_intern("receive_message");
  rb_funcall(rb_mTelegram, sym_rev_message, 1, msg);
}

VALUE send_msg_rb(VALUE self, VALUE peer, VALUE msg){
  peer_id_t c_peer;
  c_peer.id = FIX2INT(rb_iv_get(peer, "@id"));
  c_peer.type = FIX2INT(rb_iv_get(peer, "@type"));

  do_send_message (c_peer, RSTRING_PTR(msg), RSTRING_LEN(msg));
  net_connection (1, 1);

  return Qnil;
}

VALUE poll_msg_rb(VALUE self){
  net_connection(1, 0);
  return Qnil;
}

VALUE build_user_rb_obj(peer_t *peer){
  VALUE argv[0];
  VALUE user = rb_class_new_instance(0, argv, rb_cUser);

  rb_iv_set(user, "@id", INT2FIX(peer->id.id));
  rb_iv_set(user, "@type", INT2FIX(peer->id.type));
  rb_iv_set(user, "@name", rb_str_new2(peer->user.print_name));
  rb_iv_set(user, "@phone", rb_str_new2(peer->user.phone));

  return user;
}

VALUE users_list_rb(VALUE self){
  int i = 0;
  VALUE users;

  do_update_contact_list();
  net_connection (1, 1);
  net_connection(1, 0);
  net_connection(1, 0);

  users = rb_ary_new2(peer_num);

  for (i = 0; i < peer_num; i++){
    rb_ary_push(users, build_user_rb_obj(Peers[i]));
  }

  return users;
}

void Init_telegram_ext() {
  ID sym_telegram = rb_intern("Telegram");
  ID sym_peer_id = rb_intern("PeerId");
  ID sym_message = rb_intern("Message");
  ID sym_user = rb_intern("User");

  rb_mTelegram = rb_const_get(rb_cObject, sym_telegram);;
  rb_cPeerId = rb_const_get(rb_mTelegram, sym_peer_id);
  rb_cMessage = rb_const_get(rb_mTelegram, sym_message);
  rb_cUser = rb_const_get(rb_mTelegram, sym_user);

  rb_define_singleton_method(rb_mTelegram, "load_config", load_config, 1);
  rb_define_singleton_method(rb_mTelegram, "send_message", send_msg_rb, 2);
  rb_define_singleton_method(rb_mTelegram, "poll_messages", poll_msg_rb, 0);
  rb_define_singleton_method(rb_mTelegram, "contact_list", users_list_rb, 0);
}


