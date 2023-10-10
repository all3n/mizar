#ifndef _UTILS_H
#define _UTILS_H

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdio.h>

#define XLOG_DEBUG 0
#define XLOG_INFO 1
#define XLOG_WARNING 2
#define XLOG_ERROR 3
static int LOG_LEVEL = -1;
int get_log_level();
void set_log_level(const char *log_level_str);

#define XLOG(level, format, ...)                                               \
  do {                                                                         \
    int log_level = get_log_level();                                           \
    time_t t = time(NULL);                                                     \
    struct tm *tm_info = localtime(&t);                                        \
    if (XLOG_##level < log_level) {                                            \
      break;                                                                   \
    }                                                                          \
    if (XLOG_##level == XLOG_INFO) {                                           \
      printf("[%04d-%02d-%02d %02d:%02d:%02d][INFO][%s:%d] " format "\n",      \
             tm_info->tm_year + 1900, tm_info->tm_mon + 1, tm_info->tm_mday,   \
             tm_info->tm_hour, tm_info->tm_min, tm_info->tm_sec, __FILE__,     \
             __LINE__, ##__VA_ARGS__);                                         \
    } else if (XLOG_##level == XLOG_DEBUG) {                                   \
      printf("[%04d-%02d-%02d %02d:%02d:%02d][DEBUG][%s:%d] " format "\n",     \
             tm_info->tm_year + 1900, tm_info->tm_mon + 1, tm_info->tm_mday,   \
             tm_info->tm_hour, tm_info->tm_min, tm_info->tm_sec, __FILE__,     \
             __LINE__, ##__VA_ARGS__);                                         \
    } else if (XLOG_##level == XLOG_WARNING) {                                 \
      printf("[%04d-%02d-%02d %02d:%02d:%02d][WARNING][%s:%d] " format "\n",   \
             tm_info->tm_year + 1900, tm_info->tm_mon + 1, tm_info->tm_mday,   \
             tm_info->tm_hour, tm_info->tm_min, tm_info->tm_sec, __FILE__,     \
             __LINE__, ##__VA_ARGS__);                                         \
    } else if (XLOG_##level == XLOG_ERROR) {                                   \
      fprintf(stderr,                                                          \
              "[%04d-%02d-%02d %02d:%02d:%02d][ERROR][%s:%d] " format "\n",    \
              tm_info->tm_year + 1900, tm_info->tm_mon + 1, tm_info->tm_mday,  \
              tm_info->tm_hour, tm_info->tm_min, tm_info->tm_sec, __FILE__,    \
              __LINE__, ##__VA_ARGS__);                                        \
    }                                                                          \
  } while (0)

/* get user path, you should free path if your not use avoid memory leak */
char *get_user_path(const char *sub_path);
int is_dir(const char *path);
char *expanduser(const char *path);

#endif // !#ifndef _UTILS_H
