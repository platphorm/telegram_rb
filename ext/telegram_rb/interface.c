/*
    This file is part of telegram-client.

    Telegram-client is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    Telegram-client is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this telegram-client.  If not, see <http://www.gnu.org/licenses/>.

    Copyright Vitaly Valtman 2013
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define _GNU_SOURCE 

#include <assert.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "include.h"
#include "queries.h"

#include "interface.h"
#include "telegram.h"
#include "structures.h"

#include "mtproto-common.h"

#define ALLOW_MULT 1
char *default_prompt = "> ";

int unread_messages;
int msg_num_mode;
int alert_sound;

int safe_quit;

int log_level;

long long cur_uploading_bytes;
long long cur_uploaded_bytes;
long long cur_downloading_bytes;
long long cur_downloaded_bytes;

char *line_ptr;
extern peer_t *Peers[];
extern int peer_num;

int in_chat_mode;
peer_id_t chat_mode_id;


int is_same_word (const char *s, size_t l, const char *word) {
  return s && word && strlen (word) == l && !memcmp (s, word, l);
}

char *next_token (int *l) {
  while (*line_ptr == ' ') { line_ptr ++; }
  if (!*line_ptr) { 
    *l = 0;
    return 0;
  }
  int neg = 0;
  char *s = line_ptr;
  int in_str = 0;
  while (*line_ptr && (*line_ptr != ' ' || neg || in_str)) {
/*    if (*line_ptr == '\\') {
      neg = 1 - neg;
    } else {
      if (*line_ptr == '"' && !neg) {
        in_str = !in_str;
      }
      neg = 0;
    }*/
    line_ptr++;
  }
  *l = line_ptr - s;
  return s;
}

#define NOT_FOUND (int)0x80000000
peer_id_t PEER_NOT_FOUND = {.id = NOT_FOUND};

long long next_token_int (void) {
  int l;
  char *s = next_token (&l);
  if (!s) { return NOT_FOUND; }
  char *r;
  long long x = strtoll (s, &r, 10);
  if (r == s + l) { 
    return x;
  } else {
    return NOT_FOUND;
  }
}

peer_id_t next_token_user (void) {
  int l;
  char *s = next_token (&l);
  if (!s) { return PEER_NOT_FOUND; }

  if (l >= 6 && !memcmp (s, "user#", 5)) {
    s += 5;    
    l -= 5;
    int r = atoi (s);
    if (r >= 0) { return set_peer_id (PEER_USER, r); }
    else { return PEER_NOT_FOUND; }
  }

  int index = 0;
  while (index < peer_num && (!is_same_word (s, l, Peers[index]->print_name) || get_peer_type (Peers[index]->id) != PEER_USER)) {
    index ++;
  }
  if (index < peer_num) {
    return Peers[index]->id;
  } else {
    return PEER_NOT_FOUND;
  }
}

peer_id_t next_token_chat (void) {
  int l;
  char *s = next_token (&l);
  if (!s) { return PEER_NOT_FOUND; }
  
  if (l >= 6 && !memcmp (s, "chat#", 5)) {
    s += 5;    
    l -= 5;
    int r = atoi (s);
    if (r >= 0) { return set_peer_id (PEER_CHAT, r); }
    else { return PEER_NOT_FOUND; }
  }

  int index = 0;
  while (index < peer_num && (!is_same_word (s, l, Peers[index]->print_name) || get_peer_type (Peers[index]->id) != PEER_CHAT)) {
    index ++;
  }
  if (index < peer_num) {
    return Peers[index]->id;
  } else {
    return PEER_NOT_FOUND;
  }
}

peer_id_t next_token_encr_chat (void) {
  int l;
  char *s = next_token (&l);
  if (!s) { return PEER_NOT_FOUND; }

  int index = 0;
  while (index < peer_num && (!is_same_word (s, l, Peers[index]->print_name) || get_peer_type (Peers[index]->id) != PEER_ENCR_CHAT)) {
    index ++;
  }
  if (index < peer_num) {
    return Peers[index]->id;
  } else {
    return PEER_NOT_FOUND;
  }
}

peer_id_t next_token_peer (void) {
  int l;
  char *s = next_token (&l);
  if (!s) { return PEER_NOT_FOUND; }
  
  if (l >= 6 && !memcmp (s, "user#", 5)) {
    s += 5;    
    l -= 5;
    int r = atoi (s);
    if (r >= 0) { return set_peer_id (PEER_USER, r); }
    else { return PEER_NOT_FOUND; }
  }
  if (l >= 6 && !memcmp (s, "chat#", 5)) {
    s += 5;    
    l -= 5;
    int r = atoi (s);
    if (r >= 0) { return set_peer_id (PEER_CHAT, r); }
    else { return PEER_NOT_FOUND; }
  }

  int index = 0;
  while (index < peer_num && (!is_same_word (s, l, Peers[index]->print_name))) {
    index ++;
  }
  if (index < peer_num) {
    return Peers[index]->id;
  } else {
    return PEER_NOT_FOUND;
  }
}

char *modifiers[] = {
  "[offline]",
  0
};

int complete_user_list (int index, const char *text, int len, char **R) {
  index ++;
  while (index < peer_num && (!Peers[index]->print_name || strncmp (Peers[index]->print_name, text, len) || get_peer_type (Peers[index]->id) != PEER_USER)) {
    index ++;
  }
  if (index < peer_num) {
    *R = strdup (Peers[index]->print_name);
    return index;
  } else {
    return -1;
  }
}

int complete_chat_list (int index, const char *text, int len, char **R) {
  index ++;
  while (index < peer_num && (!Peers[index]->print_name || strncmp (Peers[index]->print_name, text, len) || get_peer_type (Peers[index]->id) != PEER_CHAT)) {
    index ++;
  }
  if (index < peer_num) {
    *R = strdup (Peers[index]->print_name);
    return index;
  } else {
    return -1;
  }
}

int complete_encr_chat_list (int index, const char *text, int len, char **R) {
  index ++;
  while (index < peer_num && (!Peers[index]->print_name || strncmp (Peers[index]->print_name, text, len) || get_peer_type (Peers[index]->id) != PEER_ENCR_CHAT)) {
    index ++;
  }
  if (index < peer_num) {
    *R = strdup (Peers[index]->print_name);
    return index;
  } else {
    return -1;
  }
}

int complete_user_chat_list (int index, const char *text, int len, char **R) {
  index ++;
  while (index < peer_num && (!Peers[index]->print_name || strncmp (Peers[index]->print_name, text, len))) {
    index ++;
  }
  if (index < peer_num) {
    *R = strdup (Peers[index]->print_name);
    return index;
  } else {
    return -1;
  }
}

int complete_string_list (char **list, int index, const char *text, int len, char **R) {
  index ++;
  while (list[index] && strncmp (list[index], text, len)) {
    index ++;
  }
  if (list[index]) {
    *R = strdup (list[index]);
    return index;
  } else {
    *R = 0;
    return -1;
  }
}

int offline_mode;
int count = 1;
void work_modifier (const char *s, int l) {
  if (is_same_word (s, l, "[offline]")) {
    offline_mode = 1;
  }
#ifdef ALLOW_MULT
  if (sscanf (s, "[x%d]", &count) >= 1) {
  }
#endif
}



void interpreter_chat_mode (char *line) {
  if (line == NULL || /* EOF received */
          !strncmp (line, "/exit", 5) || !strncmp (line, "/quit", 5)) {
    in_chat_mode = 0;
    update_prompt ();
    return;
  }
  if (!strncmp (line, "/history", 8)) {
    int limit = 40;
    sscanf (line, "/history %99d", &limit);
    if (limit < 0 || limit > 1000) { limit = 40; }
    do_get_history (chat_mode_id, limit);
    return;
  }
  if (!strncmp (line, "/read", 5)) {
    do_mark_read (chat_mode_id);
    return;
  }
  if (strlen (line)>0) {
    do_send_message (chat_mode_id, line, strlen (line));
  }
}

void rprintf (const char *format, ...) {
  va_list ap;
  va_start (ap, format);
  vfprintf (stdout, format, ap);
  va_end (ap);
}

void hexdump (int *in_ptr, int *in_end) {
  int *ptr = in_ptr;
  while (ptr < in_end) { printf (" %08x", *(ptr ++)); }
  printf ("\n");
}

int prompt_was;
void logprintf (const char *format, ...) {
  int x = 0;
  if (!prompt_was) {
    x = 1;
  }
  va_list ap;
  va_start (ap, format);
  vfprintf (stdout, format, ap);
  va_end (ap);
}

int color_stack_pos;
const char *color_stack[10];

void print_media (struct message_media *M) {
  assert (M);
  switch (M->type) {
    case CODE_message_media_empty:
    case CODE_decrypted_message_media_empty:
      return;
    case CODE_message_media_photo:
      if (M->photo.caption && strlen (M->photo.caption)) {
        printf ("[photo %s]", M->photo.caption);
      } else {
        printf ("[photo]");
      }
      return;
    case CODE_message_media_video:
      printf ("[video]");
      return;
    case CODE_message_media_audio:
      printf ("[audio]");
      return;
    case CODE_message_media_document:
      if (M->document.mime_type && M->document.caption) {
        printf ("[document %s: type %s]", M->document.caption, M->document.mime_type);
      } else {
        printf ("[document]");
      }
      return;
    case CODE_decrypted_message_media_photo:
       printf ("[photo]");
      return;
    case CODE_decrypted_message_media_video:
      printf ("[video]");
      return;
    case CODE_decrypted_message_media_audio:
      printf ("[audio]");
      return;
    case CODE_decrypted_message_media_document:
      printf ("[document]");
      return;
    case CODE_message_media_geo:
      printf ("[geo] https://maps.google.com/?q=%.6lf,%.6lf", M->geo.latitude, M->geo.longitude);
      return;
    case CODE_message_media_contact:
      printf ("[contact] ");
      printf ("%s %s ", M->first_name, M->last_name);
      printf ("%s", M->phone);
      return;
    case CODE_message_media_unsupported:
      printf ("[unsupported]");
      return;
    default:
      assert (0);
  }
}

int unknown_user_list_pos;
int unknown_user_list[1000];

void print_user_name (peer_id_t id, peer_t *U) {
  assert (get_peer_type (id) == PEER_USER);
  if (!U) {
    printf ("user#%d", get_peer_id (id));
    int i;
    int ok = 1;
    for (i = 0; i < unknown_user_list_pos; i++) {
      if (unknown_user_list[i] == get_peer_id (id)) {
        ok = 0;
        break;
      }
    }
    if (ok) {
      assert (unknown_user_list_pos < 1000);
      unknown_user_list[unknown_user_list_pos ++] = get_peer_id (id);
    }
  } else {
    if ((U->flags & FLAG_DELETED)) {
      printf ("deleted user#%d", get_peer_id (id));
    } else if (!(U->flags & FLAG_CREATED)) {
      printf ("empty user#%d", get_peer_id (id));
    } else if (!U->user.first_name || !strlen (U->user.first_name)) {
      printf ("%s", U->user.last_name);
    } else if (!U->user.last_name || !strlen (U->user.last_name)) {
      printf ("%s", U->user.first_name);
    } else {
      printf ("%s %s", U->user.first_name, U->user.last_name); 
    }
  }
}

void print_chat_name (peer_id_t id, peer_t *C) {
  assert (get_peer_type (id) == PEER_CHAT);
  if (!C) {
    printf ("chat#%d", get_peer_id (id));
  } else {
    printf ("%s", C->chat.title);
  }
}

void print_encr_chat_name (peer_id_t id, peer_t *C) {
  assert (get_peer_type (id) == PEER_ENCR_CHAT);
  if (!C) {
    printf ("encr_chat#%d", get_peer_id (id));
  } else {
    printf ("%s", C->print_name);
  }
}

void print_encr_chat_name_full (peer_id_t id, peer_t *C) {
  assert (get_peer_type (id) == PEER_ENCR_CHAT);
  if (!C) {
    printf ("encr_chat#%d", get_peer_id (id));
  } else {
    printf ("%s", C->print_name);
  }
}

static char *monthes[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
void print_date (long t) {
  struct tm *tm = localtime ((void *)&t);
  if (time (0) - t < 12 * 60 * 60) {
    printf ("[%02d:%02d] ", tm->tm_hour, tm->tm_min);
  } else {
    printf ("[%02d %s]", tm->tm_mday, monthes[tm->tm_mon]);
  }
}

void print_date_full (long t) {
  struct tm *tm = localtime ((void *)&t);
  printf ("[%04d/%02d/%02d %02d:%02d:%02d]", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
}

int our_id;

void print_service_message (struct message *M) {
  assert (M);
  if (msg_num_mode) {
    printf ("%lld ", M->id);
  }
  print_date (M->date);
  printf (" ");
  if (get_peer_type (M->to_id) == PEER_CHAT) {
    print_chat_name (M->to_id, user_chat_get (M->to_id));
  } else {
    assert (get_peer_type (M->to_id) == PEER_ENCR_CHAT);
    print_encr_chat_name (M->to_id, user_chat_get (M->to_id));
  }
  printf (" ");
  print_user_name (M->from_id, user_chat_get (M->from_id));
 
  switch (M->action.type) {
  case CODE_message_action_empty:
    printf ("\n");
    break;
  case CODE_message_action_geo_chat_create:
    printf ("Created geo chat\n");
    break;
  case CODE_message_action_geo_chat_checkin:
    printf ("Checkin in geochat\n");
    break;
  case CODE_message_action_chat_create:
    printf (" created chat %s. %d users\n", M->action.title, M->action.user_num);
    break;
  case CODE_message_action_chat_edit_title:
    printf (" changed title to %s\n", 
      M->action.new_title);
    break;
  case CODE_message_action_chat_edit_photo:
    printf (" changed photo\n");
    break;
  case CODE_message_action_chat_delete_photo:
    printf (" deleted photo\n");
    break;
  case CODE_message_action_chat_add_user:
    printf (" added user ");
    print_user_name (set_peer_id (PEER_USER, M->action.user), user_chat_get (set_peer_id (PEER_USER, M->action.user)));
    printf ("\n");
    break;
  case CODE_message_action_chat_delete_user:
    printf (" deleted user ");
    print_user_name (set_peer_id (PEER_USER, M->action.user), user_chat_get (set_peer_id (PEER_USER, M->action.user)));
    printf ("\n");
    break;
  case CODE_decrypted_message_action_set_message_t_t_l:
    printf (" set ttl to %d seconds. Unsupported yet\n", M->action.ttl);
    break;
  default:
    assert (0);
  }
}

peer_id_t last_from_id;
peer_id_t last_to_id;

void print_message (struct message *M) {
  assert (M);
  if (M->flags & (FLAG_MESSAGE_EMPTY | FLAG_DELETED)) {
    return;
  }
  if (!(M->flags & FLAG_CREATED)) { return; }
  if (M->service) {
    print_service_message (M);
    return;
  }
  if (!get_peer_type (M->to_id)) {
    logprintf ("Bad msg\n");
    return;
  }

  last_from_id = M->from_id;
  last_to_id = M->to_id;

  if (get_peer_type (M->to_id) == PEER_USER) {
    if (M->out) {
      if (msg_num_mode) {
        printf ("%lld ", M->id);
      }
      print_date (M->date);
      printf (" ");
      print_user_name (M->to_id, user_chat_get (M->to_id));
      if (M->unread) {
        printf (" <<< ");
      } else {
        printf (" ««« ");
      }
    } else {
      if (msg_num_mode) {
        printf ("%lld ", M->id);
      }
      print_date (M->date);
      printf (" ");
      print_user_name (M->from_id, user_chat_get (M->from_id));
      if (M->unread) {
        printf (" >>> ");
      } else {
        printf (" »»» ");
      }
      if (alert_sound) {
        play_sound();
      }
    }
  } else if (get_peer_type (M->to_id) == PEER_ENCR_CHAT) {
    peer_t *P = user_chat_get (M->to_id);
    assert (P);
    if (M->out) {
      if (msg_num_mode) {
        printf ("%lld ", M->id);
      }
      print_date (M->date);
      printf (" ");
      printf (" %s", P->print_name);
      if (M->unread) {
        printf (" <<< ");
      } else {
        printf (" ««« ");
      }
    } else {
      if (msg_num_mode) {
        printf ("%lld ", M->id);
      }
      print_date (M->date);
      printf (" %s", P->print_name);
      if (M->unread) {
        printf (" >>> ");
      } else {
        printf (" »»» ");
      }
      if (alert_sound) {
        play_sound();
      }
    }
  } else {
    assert (get_peer_type (M->to_id) == PEER_CHAT);
    if (msg_num_mode) {
      printf ("%lld ", M->id);
    }
    print_date (M->date);
    printf (" ");
    print_chat_name (M->to_id, user_chat_get (M->to_id));
    printf (" ");
    print_user_name (M->from_id, user_chat_get (M->from_id));
    if (M->unread) {
      printf (" >>> ");
    } else {
      printf (" »»» ");
    }
  }
  if (get_peer_type (M->fwd_from_id) == PEER_USER) {
    printf ("[fwd from ");
    print_user_name (M->fwd_from_id, user_chat_get (M->fwd_from_id));
    printf ("] ");
  }
  if (M->message && strlen (M->message)) {
    printf ("%s", M->message);
  }
  if (M->media.type != CODE_message_media_empty) {
    print_media (&M->media);
  }
  assert (!color_stack_pos);
  printf ("\n");
}

void play_sound (void) {
  printf ("\a");
}
