#include "mz_opts.h"
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void parse_host(const char *t_str, void *data) { *(const char **)data = t_str; }
void parse_port(const char *t_str, void *data) {
  int port = atoi(t_str);
  if (port <= 0 || port > 65535) {
    fprintf(stderr, "Invalid port: %s\n", t_str);
    exit(1);
  }
  *(int *)data = port;
}
const char *arg_type_to_str(ArgOptType t) {
  switch (t) {
  case kArgString:
    return "string";
  case kArgInt:
    return "int";
  case kArgLong:
    return "long";
  case kArgFloat:
    return "float";
  case kArgBool:
    return "bool";
  case kArgEnum:
    return "enum";
  default:
    return "unknown";
  }
}

void print_help(const char *progname, ArgOpt *opts_def) {
  fprintf(stderr, "Usage: %s [options]\n", progname);
  fprintf(stderr, "\n");
  fprintf(stderr, "Options:\n");
  for (int i = 0; opts_def[i].s_name != NULL; i++) {
    ArgOpt *opt = &opts_def[i];
    fprintf(stderr, "  %s,%s %s %s", opt->s_name, opt->l_name,
            arg_type_to_str(opt->type),
            opt->required ? "required" : "optional");
    if (opt->description != NULL) {
      fprintf(stderr, "    %s", opt->description);
    }
    fprintf(stderr, "\n");
  }
  fprintf(stderr, "\n");
}

int parse_opts(int argc, char *argv[], ArgOpt *opts_def) {
  for (int i = 0; i < argc; ++i) {
    for (int j = 0; opts_def[j].s_name != NULL; ++j) {
      if ((opts_def[j].s_name && strcmp(argv[i], opts_def[j].s_name) == 0) ||
          (opts_def[j].l_name && strcmp(argv[i], opts_def[j].l_name) == 0)) {
        if (opts_def[j].type == kArgBool) {
          *(bool *)opts_def[j].data = true;
        } else {
          if (i + 1 < argc) {
            const char *v = argv[++i];
            if (v[0] == '-') {
              fprintf(stderr, "Invalid argument: %s %s\n", argv[i - 1],
                      argv[i]);
              return -1;
            }
            if (opts_def[j].func) {
              opts_def[j].func(v, opts_def[j].data);
            } else {
              if (opts_def[j].type == kArgString) {
                *(const char **)opts_def[j].data = v;
              } else if (opts_def[j].type == kArgInt) {
                *(int *)opts_def[j].data = atoi(v);
              } else if (opts_def[j].type == kArgFloat) {
                *(double *)opts_def[j].data = atof(v);
              } else if (opts_def[j].type == kArgLong) {
                *(long *)opts_def[j].data = atol(v);
              }
            }
          } else {
            fprintf(stderr, "Option %s requires an argument\n",
                    opts_def[j].s_name);
            return -1;
          }
        }
        break;
      }
    }
  }
  /*
  if (opts->help) {
    print_help(argv[0], opts_def);
    return -1;
  }
  if (opts->verbose) {
  }
  */
  return 0;
}


