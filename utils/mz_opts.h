#ifndef _MZ_OPTS_H
#define _MZ_OPTS_H
#include <stdbool.h>

typedef enum ArgOptType {
  kArgString = 0,
  kArgInt,
  kArgLong,
  kArgFloat,
  kArgBool,
  kArgEnum,
} ArgOptType;

typedef void (*ParseFunc)(const char *, void *data);
typedef struct ArgOpt {
  void *data;
  char *s_name;
  char *l_name;
  ArgOptType type;
  bool required;
  char *description;
  ParseFunc func;
} ArgOpt;

#define ARG_END                                                                \
  { NULL, NULL, NULL, 0, false, NULL, NULL }

#define ARG_BOOL(s_name, l_name, ...)                                          \
  { &opts.l_name, "-" #s_name, "--" #l_name, .type = kArgBool, __VA_ARGS__ }

#define ARG_STR(s_name, l_name, ...)                                           \
  { &opts.l_name, "-" #s_name, "--" #l_name, .type = kArgString, __VA_ARGS__ }
#define ARG_INT(s_name, l_name, ...)                                           \
  { &opts.l_name, "-" #s_name, "--" #l_name, .type = kArgInt, __VA_ARGS__ }
#define ARG_ENUM(s_name, l_name, ...)                                          \
  { &opts.l_name, "-" #s_name, "--" #l_name, .type = kArgEnum, __VA_ARGS__ }

int parse_opts(int argc, char *argv[], ArgOpt *opts_def);
void parse_host(const char *t_str, void *data);
void parse_port(const char *t_str, void *data);
void parse_type(const char *t_str, void *data);
void print_help(const char *progname, ArgOpt *opts_def);

#define PARSE_MAIN_ARGS(argc, argv, opts_def)                                  \
  if (parse_opts(argc, argv, opts_def)) {                                      \
    print_help(argv[0], opts_def);                                             \
    return -1;                                                                 \
  }                                                                            \
  if (opts.help) {                                                             \
    print_help(argv[0], opts_def);                                             \
    return 0;                                                                  \
  } else if (opts.verbose) {                                                   \
    print_opts(&opts);                                                         \
  }


#endif
