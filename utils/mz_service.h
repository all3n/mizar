#ifndef _MZ_SERVICE_H
#define _MZ_SERVICE_H
#include "app_def.h"
#include "mz_opts.h"
#include "mz_sockets.h"

typedef void (*SvcFunc)(MzOpts *opts);
typedef struct MzSvc {
  const char *name;
  SvcFunc func;
} MzSvc;

void run_server(MzOpts *opts);
void run_client(MzOpts *opts);
void *client_thread(void *arg);
void run_svc(const char *name, MzOpts *opts);
#endif
