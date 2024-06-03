#include "cpm.h"
#define SUBMODCOUNT 2
#define SUBMODFOLDER "./example/"

const char *submodules[] = {"basic", "assembly"};

void bld(Arguments args) {
  Path a = cpm_path_from_cstr("./example");
  if (args.count < 2) {
    cpm_log(CPM_ERROR, "no submodule selected\n");
    cpm_log(CPM_MSG, "available submodules:\n");
    for (int i = 0; i < SUBMODCOUNT; i++) {
      cpm_log(CPM_MSG, "%s\n", submodules[i]);
    }
    return;
  }
  for (int i = 0; i < SUBMODCOUNT; i++) {
    if (!strcmp(args.array[1]->str, submodules[i])) {
      cpm_path_append_cstr(&a, args.array[1]->str);
      cpm_submodule(a.str, NULL);
      return;
    }
  }
  cpm_log(CPM_ERROR, "submodule %s does not exist\n", args.array[1]->str);
  cpm_log(CPM_MSG, "submodules available:\n");
  for (int i = 0; i < SUBMODCOUNT; i++) {
    cpm_log(CPM_MSG, "%s\n", submodules[i]);
  }
  return;
}

void cln(Arguments _args){
  cpm_log(CPM_INFO, "rm ./example/assembly/build\n");
  cpm_rm(SUBMODFOLDER"/assembly/build");
  cpm_log(CPM_INFO, "rm ./example/assembly/buildscript\n");
  cpm_rm(SUBMODFOLDER"/assembly/buildscript");
  cpm_log(CPM_INFO, "rm ./example/basic/buildscript\n");
  cpm_rm(SUBMODFOLDER"/basic/buildscript");
  
}

const CliCommand build = {
    .desc = "builds the project according to submodules selected", .name = "build", .function = bld};
const CliCommand clean = {.desc = "cleans the entire project", .name = "clean", .function = cln};


int main(int argc, char **argv) {
  CPM_REBUILD_SELF(argc, argv);
  CliEnv clienv = cpm_create_cliEnv_Cargs(argc, argv);
  cpm_append_env_commands(&clienv, build);
  cpm_append_env_commands(&clienv, clean);
  cpm_CLI(clienv);
  cpm_free_env(clienv);
  return 0;
}
