#include "utils/app_def.h"
#include "utils/mz_opts.h"
#include "utils/mz_service.h"
#include "utils/utils.h"
extern MzOpts opts;
extern ArgOpt opts_def[];

int main(int argc, char *argv[]) {
  PARSE_MAIN_ARGS(argc, argv, opts_def);
  set_log_level(opts.log_level);
  run_svc(opts.type, &opts);
  return 0;
}
