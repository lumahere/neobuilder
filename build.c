#include "cpm.h"

int main(void) {
  Cmd a = {0};
  cpm_cmd_append(&a, "clang");
  cpm_cmd_append(&a, "main.c");
  cpm_cmd_append(&a, "-o", "main");
  cpm_log(CPM_INFO, "Building with cmd: %s", a.str);
  cpm_cmd_exec(a);
}
