#ifndef _APP_DEF_H
#define _APP_DEF_H
#include <stdbool.h>

typedef enum MzSvcType { kSvcServer, kSvcClient } MzSvcType;
typedef struct MzOpts {
  bool help;
  bool verbose;
  const char *type;
  const char *host;
  int port;
  const char *log_level;
} MzOpts;

void parse_type(const char *t_str, void *data);
const char *svc_type_to_str(MzSvcType t);
void print_opts(const MzOpts *opts);

#endif
