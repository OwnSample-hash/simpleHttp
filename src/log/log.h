/**
 * @file
 * @brief Loggin system
 *
 * Copyright (c) 2020 rxi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See `log.c` for details.
 */

#ifndef LOG_H
#define LOG_H

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>

#define LOG_VERSION "0.1.0"

/**
 * @brief Represent a log event
 *
 * @var log_event::ap
 * Varidactic arguments list
 *
 * @var log_event::fmt
 * The fmt to print by
 *
 * @var log_event::file
 * Which source file it comes from
 *
 * @var log_event::time
 * Time of the event
 *
 * @var log_event::udata
 * User data
 *
 * @var log_event::line
 * In which line it comes
 *
 * @var log_event::level
 * The level of the event
 */
typedef struct log_event {
  va_list ap;
  const char *fmt;
  const char *file;
  struct tm *time;
  void *udata;
  int line;
  int level;
} log_event_t;
/**<@copydoc log_event*/

/**
 * @brief Callback when logging
 */
typedef void (*log_LogFn)(log_event_t *ev);

/**
 * @brief Function pointer called for custom lock
 */
typedef void (*log_LockFn)(bool lock, void *udata);

/**
 * @brief Levels for logging
 */
typedef enum _LVLS {
  LOG_TRACE,
  LOG_DEBUG,
  LOG_INFO,
  LOG_WARN,
  LOG_ERROR,
  LOG_FATAL
} LOG_LEVEL;

/**
 * @brief
 *
 * @def log_trace(...)
 * @brief Log a trace event
 *
 * @def log_debug(...)
 * @brief Log a debug event
 *
 * @def log_info(...)
 * @brief Log a info event
 *
 * @def log_warn(...)
 * @brief Log a warn event
 *
 * @def log_error(...)
 * @brief Log a error event
 *
 * @def log_fatal(...)
 * @brief Log a fatal event also aborts
 *
 */
#ifndef NO_LOG_AT_ALL
#ifndef _PLUGIN_
#define log_trace(...) log_log(LOG_TRACE, __FILE__, __LINE__, __VA_ARGS__)
#define log_debug(...) log_log(LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define log_info(...) log_log(LOG_INFO, __FILE__, __LINE__, __VA_ARGS__)
#define log_warn(...) log_log(LOG_WARN, __FILE__, __LINE__, __VA_ARGS__)
#define log_error(...) log_log(LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define log_fatal(...) log_log(LOG_FATAL, __FILE__, __LINE__, __VA_ARGS__)
#else
#define log_trace(...)                                                         \
  plugin_info->log_log(LOG_TRACE, __FILE__, __LINE__, __VA_ARGS__)
#define log_debug(...)                                                         \
  plugin_info->log_log(LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define log_info(...)                                                          \
  plugin_info->log_log(LOG_INFO, __FILE__, __LINE__, __VA_ARGS__)
#define log_warn(...)                                                          \
  plugin_info->log_log(LOG_WARN, __FILE__, __LINE__, __VA_ARGS__)
#define log_error(...)                                                         \
  plugin_info->log_log(LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define log_fatal(...)                                                         \
  plugin_info->log_log(LOG_FATAL, __FILE__, __LINE__, __VA_ARGS__)
#endif
#else

#define log_trace(...)
#define log_debug(...)
#define log_info(...)
#define log_warn(...)
#define log_error(...)
#define log_fatal(...)

#endif

/**
 * @brief Converts a log level into a string
 *
 * @param level Level to convert
 * @return The string
 */
const char *log_level_string(int level);

/**
 * @brief Set custom lock?
 *
 * @param fn Callback?
 * @param udata User data?
 */
void log_set_lock(log_LockFn fn, void *udata);

/**
 * @brief Sets the level to log by
 *
 * @param level Level to log by
 */
void log_set_level(int level);

/**
 * @brief Set quiet logging
 *
 * @param enable Enable it or not
 */
void log_set_quiet(bool enable);

/**
 * @brief Enable or disable thread id in the log
 *
 * @param enable Enable it or not
 */
void log_enable_thread_id(bool enable);

/**
 * @brief Add an additional callback when a log event happens
 *
 * @param fn The callback
 * @param udata User data
 * @param level Which levels to happen
 * @return Returns 0 if successfull -1 otherwise
 */
int log_add_callback(log_LogFn fn, void *udata, int level);

/**
 * @brief Add a FILE* to the logging outputs
 *
 * @param fp FILE* to log to
 * @param level Till which level to log
 * @return Returns 0 if successfull -1 otherwise
 */
int log_add_fp(FILE *fp, int level);

typedef void (*log_log_fn)(int level, const char *file, int line,
                           const char *fmt, ...);
/**
 * @brief Main log function used to log an event use the macros instead
 *
 * @param level Level of the event
 * @param file Source file it came from
 * @param line Line in the source file
 * @param fmt Format to print
 */
#ifndef _PLUGIN_
void log_log(int level, const char *file, int line, const char *fmt, ...);
#endif

#endif
// Vim: set expandtab tabstop=2 shiftwidth=2:
