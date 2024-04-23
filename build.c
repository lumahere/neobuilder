#include "cpm.h"

const char* build_desc = "build the entire project";
void build(){
  cpm_submodule("./example", NULL);
}


int main(int argc, char **argv) {
  CliCommand bld = cpm_create_cli_command("build", build_desc, &build);
  CliCommandArray cmdarr = {0};
  cpm_append_cli_cmd_arr(&cmdarr, bld);
  CliEnv env = cpm_create_cliEnv(cmdarr, argc, argv);
  cpm_CLI(env);
  
  return 0;
}
