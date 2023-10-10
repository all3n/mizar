#include "app_def.h"
#include "mz_opts.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void parse_type(const char *t_str, void *data) { *(const char **)data = t_str; }

const char *svc_type_to_str(MzSvcType t) {
  switch (t) {
  case kSvcServer:
    return "server";
  case kSvcClient:
    return "client";
  default:
    return "unknown";
  }
}

void print_opts(const MzOpts *opts) {
  printf("Help: %d\n", opts->help);
  printf("verbose: %d\n", opts->verbose);
  printf("Host: %s\n", opts->host);
  printf("Port: %d\n", opts->port);
  printf("Type: %s\n", opts->type);
  printf("Log level: %s\n", opts->log_level);
}

MzOpts opts = {
    .host = "127.0.0.1", .port = 8787, .log_level = "DEBUG", .type = "client"};
ArgOpt opts_def[] = {
    ARG_BOOL(h, help),
    ARG_BOOL(v, verbose),
    ARG_STR(H, host, .func = parse_host,
            .description = "client:target server host"),
    ARG_INT(p, port, .func = parse_port,
            .description = "client:target server port/server:port to listen"),
    ARG_STR(t, type, .func = parse_type,
            .description = "service type server/client"),
    ARG_STR(l, log_level, .description = "log level DEBUG/INFO/WARNING/ERROR"),
    ARG_END};
