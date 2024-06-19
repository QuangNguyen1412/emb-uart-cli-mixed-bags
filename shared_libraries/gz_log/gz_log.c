/**
 * @file gz_log.c
 * @brief 
 * @version 0.1
 * @date 2022-09-02
 * 
 * @copyright Copyright (c) Goal Zero 2022
 */

#include <stdio.h>
#include <stdarg.h>
#include "gz_log.h"

#ifdef __cplusplus
extern "C" {
#endif

static int _min_log_level = GZ_LOG_LEVEL_INFO;
static const char *_level_strings[] = {
  "ALL", "DEBUG", "INFO", "WARNING", "ERROR"
};

void __gz_log(const char* file, const char* func, int line, int level, const char* format, ...) {
  if (level < _min_log_level) {
    return;
  }
  va_list arg;

  va_start (arg, format);
  if (level == GZ_LOG_LEVEL_DEBUG) {
    printf("%s-", file);
    printf("%s()-", func);
    printf("%d-", line);
  }
  printf("%s: ", _level_strings[level]);
  vprintf(format, arg);
  va_end(arg);
}

void gz_log_set_level(int level) {
  if (level > GZ_LOG_LEVEL_ERROR) {
    _min_log_level = GZ_LOG_LEVEL_ERROR;
  } else if (level < GZ_LOG_LEVEL_ALL) {
    _min_log_level = GZ_LOG_LEVEL_ALL;
  } else {
    _min_log_level = level;
  }
}

#ifdef __cplusplus
}
#endif
