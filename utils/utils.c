#include "utils.h"
#ifdef __unix__
#include <pwd.h>
#endif
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

void set_log_level(const char *log_level_str) {
  if (log_level_str != NULL) {
    if (strcasecmp(log_level_str, "DEBUG") == 0) {
      LOG_LEVEL = XLOG_DEBUG;
    } else if (strcasecmp(log_level_str, "INFO") == 0) {
      LOG_LEVEL = XLOG_INFO;
    } else if (strcasecmp(log_level_str, "WARNING") == 0) {
      LOG_LEVEL = XLOG_WARNING;
    } else if (strcasecmp(log_level_str, "ERROR") == 0) {
      LOG_LEVEL = XLOG_ERROR;
    } else {
      LOG_LEVEL = atoi(log_level_str);
    }
  } else {
    LOG_LEVEL = XLOG_INFO;
  }
}
int get_log_level() {
  if (LOG_LEVEL == -1) {
    char *log_debug = getenv("DEBUG");
    if (log_debug != NULL && (strcasecmp(log_debug, "1") == 0 ||
                              strcasecmp(log_debug, "true") == 0)) {
      LOG_LEVEL = XLOG_DEBUG;
      return LOG_LEVEL;
    }
    char *log_level_str = getenv("LOG_LEVEL");
    set_log_level(log_level_str);
  }
  return LOG_LEVEL;
}
char *get_user_path(const char *sub_path) {
  char *home_dir = getenv("HOME"); // Linux 和 Mac OS X
  if (home_dir == NULL) {
    home_dir = getenv("USERPROFILE"); // Windows
  }
  if (home_dir == NULL) {
    return NULL;
  }
  char *path = (char *)malloc(strlen(home_dir) + strlen(sub_path) + 1);
  strcpy(path, home_dir);
  strcat(path, sub_path);
  return path;
}

int is_dir(const char *path) {
  if (path == NULL) {
    return 0;
  }
  struct stat path_stat;
  if (stat(path, &path_stat) == 0) {
    if (S_ISDIR(path_stat.st_mode)) {
      return 1;
    } else {
      return 0;
    }
  }
  return 0;
}

char *expanduser(const char *path) {
  if (path == NULL)
    return NULL;

  if (path[0] == '~') {
    const char *home = getenv("HOME");

    #ifdef __unix__
    if (home == NULL) {
      struct passwd *pw = getpwuid(getuid());
      if (pw != NULL)
        home = pw->pw_dir;
    }
    #endif

    if (home != NULL) {
      size_t len = strlen(home) + strlen(path) - 1;
      char *expanded_path = (char *)malloc(len + 1);
      sprintf(expanded_path, "%s%s", home, path + 1);
      return expanded_path;
    }
  }

  return strdup(path);
}
