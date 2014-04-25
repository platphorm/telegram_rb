#ifndef __MAIN_H__
#define __MAIN_H__

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>
#include <termios.h>
#include <unistd.h>
#include <assert.h>

#include <sys/stat.h>
#include <time.h>
#include <fcntl.h>

#ifdef HAVE_EXECINFO_H
#include <execinfo.h>
#endif
#include <signal.h>
#ifdef HAVE_LIBCONFIG
#include <libconfig.h>
#endif

#include "telegram.h"
#include "loop.h"
#include "mtproto-client.h"
#include "interface.h"
#include "tools.h"
#include "constants.h"

int telegram_main();

#endif
