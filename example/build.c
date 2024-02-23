#include "../cpm.h"

const char *build_path = "./builds/";
const char *tgt_name = "./builds/main";
const char *src = "./src/main.c";

int main(void) {
  cpm_mkdir(build_path);

  Cmd build = {0};
  cpm_cmd_append(&build, "cc");
  cpm_cmd_append(&build, src);
  cpm_cmd_append(&build, "-o", tgt_name);
  return 0;
}
