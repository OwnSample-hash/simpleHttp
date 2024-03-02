#ifndef __QUIT_HANDLER_HTTP__
#define __QUIT_HANDLER_HTTP__

#include <unistd.h>

#include "lua/setup.h"

void ctrl_c_h(int sigframe);
extern driver *drv;
#endif // !__QUIT_HANDLER_HTTP__
