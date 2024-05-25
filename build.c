#define CPM_NO_INTERACTIVE
#include "cpm.h"

void bld(Arguments args){
  if(args.count < 2){
    cpm_log(CPM_ERROR, "no submodule selected\n");
    cpm_log(CPM_MSG, "available submodules:\n");
    cpm_log(CPM_MSG, "basic\n");
    return;
    }
  if(!strcmp(args.array[1]->str, "basic")){
    cpm_submodule("./example/basic", NULL);
  } else {
    cpm_log(CPM_ERROR, "submodule %s does not exist\n", args.array[1]->str);
  }
}

CliCommand build = {.desc = "builds the project", .name="build", .function=bld};
int main(int argc, char** argv){
  CPM_REBUILD_SELF(argv);
  CliEnv clienv = cpm_create_cliEnv_Cargs(argc, argv);
  cpm_append_env_commands(&clienv, build);
  cpm_CLI(clienv);
  cpm_free_env(clienv);
}