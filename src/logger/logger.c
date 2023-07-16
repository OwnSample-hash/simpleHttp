#include "logger.h"

ELogLevel curr_level;
struct pollfd *fds;
FILE *main_fp;

void handle() {
  // being run a fork, spawns threads to read io then lock ? perhaps atomic ?
  // then write to main stdout and/or a configured FILE*
  if (curr_level == LNONE)
    return;
  printf("Polling with %d fds\n", curr_level);
  while (true) {
    int poll_ret = 0;
    while ((poll_ret = poll(fds, curr_level, -1)) > 0) {
      for (int i = curr_level; i > 0; i--) {
        if (fds[i].revents & POLLIN) {
          char *buf = calloc(MB_10, sizeof(char));
          if (read(fds[i].fd, buf, MB_10) < 0) {
            perror("log,POLLIN,read");
          };
          fprintf(stdout, "%s\n", buf);
        }
        if (fds[i].revents & POLLPRI) {
          char *buf = calloc(MB_10, sizeof(char));
          if (read(fds[i].fd, buf, MB_10) < 0) {
            perror("log,POLLPRI,read");
          };
          fprintf(main_fp, "!%s\n", buf);
        }

        if (fds[i].revents & POLLHUP) {
          fprintf(stdout, "Error fd:%d, disconnected\n", i);
        }
      }
    }
    if (poll_ret == -1) {
      perror("log, poll");
    }
  }
  free(fds);
}

void __init_err() {
  init_err();
  fds[0].fd = pipe_err[0];
  fds[0].events = POLLIN | POLLPRI;
}

void __init_warn() {
  __init_err();
  init_warn();
  fds[1].fd = pipe_warn[0];
  fds[1].events = POLLIN | POLLPRI;
}

void __init_info() {
  __init_warn();
  init_info();
  fds[2].fd = pipe_info[0];
  fds[2].events = POLLIN | POLLPRI;
}

void __init_dbg() {
  __init_info();
  init_dbg();
  fds[3].fd = pipe_dbg[0];
  fds[3].events = POLLIN | POLLPRI;
}

int init_logger(const ELogLevel lvl, const char *fn) {
  if (lvl == LNONE) {
    return 1;
  }
  if (fn != NULL) {
    main_fp = fopen(fn, "w");
  } else {
    main_fp = NULL;
  }
  curr_level = lvl;
  fds = calloc(curr_level, sizeof(struct pollfd));
  if (fds == NULL) {
    perror("log, Malloc");
    return -1;
  }
  switch (lvl) {
  case LNONE:
    break;
  case ERR:
    __init_err();
    break;
  case WARN:
    __init_warn();
    break;
  case INFO:
    __init_info();
    break;
  case DBG:
    __init_dbg();
    break;
  default:
    fprintf(stderr, "[ERR]\tIdk man shit went sour real fast\n");
    break;
  }
  return 0;
}
