#ifndef __QUIT_HANDLER_HTTP__
#define __QUIT_HANDLER_HTTP__

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "log/log.h"

void ctrl_c_h(int sigframe);

#endif // !__QUIT_HANDLER_HTTP__
