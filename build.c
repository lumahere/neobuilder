#include "cpm.h"

const char* build_desc = "build the project submodule. available sub: multi-source, basic";
void build(Arguments args){
  int submodcnt = 2;
  const char* submods[] = {"multi-source", "basic"};
  if (args.count < 3){
    cpm_log(CPM_ERROR, "command: build. missing submodule\nusage: build [submodule]\nsubmodules:\n");
  } else{
    for(int i=0; i < submodcnt; i++){
      if(cpm_strcmp(args.array[2]->str, submods[i])){
          cpm_submodule("./example", submods[i]);
          break;
      }
    }
  
}}


int main(int argc, char **argv) {
  CliCommand bld = cpm_create_cli_command("build", build_desc, &build);
  CliCommandArray cmdarr = {0};
  cpm_append_cli_cmd_arr(&cmdarr, bld);
  CliEnv env = cpm_create_cliEnv_Cargs(cmdarr, argc, argv);
  cpm_CLI(env);
  
  return 0;
}
