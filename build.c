#include "cpm.h"

int main(int argc, char **argv) {
  CPM_REBUILD_SELF(argv);
  Cmd a = {0};
  cpm_cmd_append(&a, "clang");
  cpm_cmd_append(&a, "main.c");
  cpm_cmd_append(&a, "-o", "mains");
  cpm_log(CPM_INFO, "Building with cmd: %s", a.str);
  cpm_cmd_exec(a);
}
