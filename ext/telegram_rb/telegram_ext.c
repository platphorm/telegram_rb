#include <ruby.h>
#include "main.h"

static VALUE rb_mTelegram;
static VALUE rb_cPeerId;
static VALUE rb_cMessage;
static VALUE rb_cPhoto;
static VALUE rb_cUser;
static ID sym_recv_msg;
static ID sym_poll_queue;
extern peer_t *Peers[];
extern int peer_num;

static VALUE load_config(VALUE self, VALUE pub_key, VALUE telegram_dir){
  Check_Type(pub_key, T_STRING);
  Check_Type(telegram_dir, T_STRING);

  telegram_main(RSTRING_PTR(pub_key), RSTRING_PTR(telegram_dir));

  return Qtrue;
}

VALUE build_peer_rb_obj(peer_id_t peer){
  VALUE argv[2];

  argv[0] = INT2FIX(peer.type);
  argv[1] = INT2FIX(peer.id);

  return rb_class_new_instance(2, argv, rb_cPeerId);
}

peer_id_t peer_rb_to_cstruct(VALUE peer) {
  peer_id_t peer_cs;

  peer_cs.id = FIX2INT(rb_iv_get(peer, "@id"));
  peer_cs.type = FIX2INT(rb_iv_get(peer, "@type"));

  return peer_cs;
}

VALUE build_photo_rb_obj(struct photo *photo, int next){
  VALUE argv[0];
  VALUE rb_photo = rb_class_new_instance(0, argv, rb_cPhoto); 
  int max = -1;
  int maxi = 0;
  int i;

  for (i = 0; i < photo->sizes_num; i++) {
    if (photo->sizes[i].w + photo->sizes[i].h > max) {
      max = photo->sizes[i].w + photo->sizes[i].h;
      maxi = i;
    }
  }

  rb_iv_set(rb_photo, "@w", INT2FIX(photo->sizes[maxi].w));
  rb_iv_set(rb_photo, "@h", INT2FIX(photo->sizes[maxi].h));
  rb_iv_set(rb_photo, "@volume", LL2NUM(photo->sizes[maxi].loc.volume));
  rb_iv_set(rb_photo, "@local_id", INT2FIX(photo->sizes[maxi].loc.local_id));
  rb_iv_set(rb_photo, "@size", INT2FIX(photo->sizes[maxi].size));

  return rb_photo;
}

void tel_new_msg(struct message *M, int fn){

  if (M->out){
    return;
  }

  VALUE argv[0];
  VALUE msg = rb_class_new_instance(0, argv, rb_cMessage);
  int media_type;

  rb_iv_set(msg, "@id", LONG2NUM(M->id));
  rb_iv_set(msg, "@flags", INT2FIX(M->flags));
  rb_iv_set(msg, "@date", LONG2NUM(M->date));
  rb_iv_set(msg, "@text", rb_str_new2(M->message));
  rb_iv_set(msg, "@from_id", build_peer_rb_obj(M->from_id));
  rb_iv_set(msg, "@to_id", build_peer_rb_obj(M->to_id));

  switch (M->media.type) {
  case CODE_message_media_empty:
    media_type = 1;
    break;
  case CODE_message_media_photo:
    media_type = 2;
    do_load_photo (&M->media.photo, 1);
    rb_iv_set(msg, "@media", build_photo_rb_obj(&M->media.photo, 1));
    break;
  case CODE_message_media_video:
    media_type = 3;
    break;
  case CODE_message_media_audio:
    media_type = 4;
    break;
  case CODE_message_media_document:
    media_type = 5;
    break;
  case CODE_message_media_geo:
    media_type = 6;
    break;
  case CODE_message_media_contact:
    media_type = 7;
    break;
  }

  rb_iv_set(msg, "@media_type", INT2FIX(media_type));

  //printf("****** Funtion number: %d \n ", fn);
  rb_funcall(rb_mTelegram, sym_recv_msg, 1, msg);
}

//VALUE send_msg_rb(int argc, VALUE* argv, VALUE self){
VALUE send_msg_rb(VALUE self, VALUE peer, VALUE msg_or_file, VALUE type){
  peer_id_t c_peer;
  int c_type;
  char *msg;

  if(!msg_or_file){
    return Qfalse;
  }

  msg = tstrdup(RSTRING_PTR(msg_or_file)); 

  c_peer.id = FIX2INT(rb_iv_get(peer, "@id"));
  c_peer.type = FIX2INT(rb_iv_get(peer, "@type"));
  c_type = NUM2INT(type); 

  if(c_type == 1){
    do_send_message (c_peer, msg, RSTRING_LEN(msg_or_file));
  }else if(c_type == 2){
    do_send_photo (CODE_input_media_uploaded_photo, c_peer, msg);
  //}else if(c_type == 3 || c_type == 4){
  }else if(c_type == 3){
    do_send_photo (CODE_input_media_uploaded_video, c_peer, msg);
  }else{
    return Qfalse;
  }

  //net_connection (1, 1);

  return Qtrue;
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

  users = rb_ary_new2(peer_num);

  for (i = 0; i < peer_num; i++){
    rb_ary_push(users, build_user_rb_obj(Peers[i]));
  }

  return users;
}

VALUE add_contact_rb(VALUE self, VALUE phone, VALUE first_name, VALUE last_name, VALUE force){

  do_add_contact(RSTRING_PTR(phone), RSTRING_LEN(phone), RSTRING_PTR(first_name), RSTRING_LEN(first_name), RSTRING_PTR(last_name), RSTRING_LEN(last_name), FIX2INT(force));
  //net_connection (1, 1);

  return Qnil;
}

VALUE mark_message_as_read(VALUE self) {
  peer_id_t peer;

  peer = peer_rb_to_cstruct(rb_iv_get(self, "@from_id"));
  //do_messages_mark_read(peer, FIX2INT(rb_iv_get(self, "@id")) );
  do_messages_mark_read(peer, NUM2LL(rb_iv_get(self, "@id")) );

  return Qtrue;  
}

void poll_messages_queue(){
  rb_funcall(rb_mTelegram, sym_poll_queue, 0);
}

VALUE start_loop_rb(VALUE self){
  main_loop ();
}

void Init_telegram_ext() {
  ID sym_telegram = rb_intern("Telegram");
  ID sym_peer_id = rb_intern("PeerId");
  ID sym_message = rb_intern("Message");
  ID sym_user = rb_intern("User");
  ID sym_photo = rb_intern("Photo");
  sym_recv_msg = rb_intern("receive_message");
  sym_poll_queue = rb_intern("poll_messages_queue");

  rb_mTelegram = rb_const_get(rb_cObject, sym_telegram);;
  rb_cPeerId = rb_const_get(rb_mTelegram, sym_peer_id);
  rb_cMessage = rb_const_get(rb_mTelegram, sym_message);
  rb_cUser = rb_const_get(rb_mTelegram, sym_user);
  rb_cPhoto = rb_const_get(rb_mTelegram, sym_photo);

  rb_define_singleton_method(rb_mTelegram, "load_config", load_config, 2);
  rb_define_singleton_method(rb_mTelegram, "send_message", send_msg_rb, 3);
  rb_define_singleton_method(rb_mTelegram, "poll_messages", poll_msg_rb, 0);
  rb_define_singleton_method(rb_mTelegram, "contact_list", users_list_rb, 0);
  rb_define_singleton_method(rb_mTelegram, "add_contact", add_contact_rb, 4);
  rb_define_singleton_method(rb_mTelegram, "start", start_loop_rb, 0);

  rb_define_method(rb_cMessage, "mark_read", mark_message_as_read, 0);
  
}


