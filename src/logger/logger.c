#include "logger.h"

ELogLevel curr_level;
struct pollfd *fds;
FILE *main_fp;
bool __quit;

void handle() {
  if (curr_level == LNONE)
    return;
  printf("[Pre-Info] Polling with %d fds\n", curr_level);
  while (true) {
    int poll_ret = 0;
    while ((poll_ret = poll(fds, curr_level, -1)) > 0) {
      for (int i = curr_level - 1; i > -1; i--) {
        if (fds[i].revents & (POLLIN | POLLPRI)) {
          char *buf = calloc(KB_1 * 4, sizeof(char));
          buf == NULL ? perror("log,calloc") : true;
          int readb = 0;
          if ((readb = read(fds[i].fd, buf, KB_1 * 4)) > 0)
            fprintf(main_fp, "%s", buf);
          else
            perror("log,read");
          buf[0] = 0;
          free(buf);
        } else if (fds[i].revents & POLLHUP)
          fprintf(stderr, "Error fd:%d, disconnected\n", i);
      }
    }
    if (poll_ret == -1)
      perror("log,poll");
  }
}

void __init_err() {
  init_err(__quit);
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

int init_logger(const logConf_t *conf) {
  if (conf->lvl == LNONE) {
    return 1;
  }
  if (conf->fn != NULL) {
    main_fp = fopen(conf->fn, "w");
    if (!main_fp) {
      perror("log,fn,fopen");
      fprintf(stderr, "Faling back to stdout!\n");
      main_fp = stdout;
    }
  } else if (conf->fp != NULL) {
    main_fp = conf->fp;
  } else {
    main_fp = stdout;
  }
  if (!main_fp) {
    fprintf(stderr, "main_fp = 0x0\n");
    return 1;
  }
  __quit = conf->errQuit;

  curr_level = conf->lvl;
  fds = calloc(curr_level, sizeof(struct pollfd));
  if (fds == NULL) {
    perror("log, Malloc");
    return -1;
  }
  switch (conf->lvl) {
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
