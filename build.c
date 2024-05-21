#define CPM_NO_CLI
#include "cpm.h"

const char *build_desc =
    "build the project submodule. available sub: multi-source, basic";

void build(Arguments args) {
  int submodcnt = 2;
  const char *submods[] = {"multi-source", "basic"};
  Arguments pass_args = {0};
  cpm_append_arguments(&pass_args, "build");
  if (args.count < 3) {
    cpm_log(CPM_ERROR, "command: build. missing submodule\nusage: build "
                       "[submodule]\nsubmodules:\n");
    for (int i = 0; i < submodcnt; i++) {
      cpm_log(CPM_MSG, "%s\n", submods[i]);
    }
    return;
  } else {
    for (int i = 0; i < submodcnt; i++) {
      if (cpm_strcmp(args.array[2]->str, submods[i])) {
        cpm_append_arguments(&pass_args, submods[i]);
        cpm_submodule("./example", &pass_args);
        return;
      }
    }
    cpm_log(CPM_ERROR, "command: build. incorrect submodule\nusage: build "
                       "[submodule]\nsubmodules:\n");
    for (int i = 0; i < submodcnt; i++) {
      cpm_log(CPM_MSG, "%s\n", submods[i]);
    }
  }
}

void run(Arguments args) {
  int submodcnt = 2;
  const char *submods[] = {"multi-source", "basic"};
  Arguments pass_args = {0};
  cpm_append_arguments(&pass_args, "run");
  if (args.count < 3) {

    cpm_log(CPM_ERROR, "command: run. missing submodule\nusage: run "
                       "[submodule]\nsubmodules:\n");
    for (int i = 0; i < submodcnt; i++) {
      cpm_log(CPM_MSG, "%s\n", submods[i]);
    }
    return;
  } else {
    for (int i = 0; i < submodcnt; i++) {
      if (cpm_strcmp(args.array[2]->str, submods[i])) {
        cpm_append_arguments(&pass_args, submods[i]);
        cpm_submodule("./example", &pass_args);
        return;
      }
    }
    cpm_log(CPM_ERROR, "command: run. incorrect submodule\nusage: run "
                       "[submodule]\nsubmodules:\n");
    for (int i = 0; i < submodcnt; i++) {
      cpm_log(CPM_MSG, "%s\n", submods[i]);
    }
  }
}

int main(int argc, char **argv) {
  CPM_REBUILD_SELF(argv)
  CliCommand bld = cpm_create_cli_command("build", build_desc, &build);
  CliEnv env = cpm_create_cliEnv_Cargs(argc, argv);
  cpm_append_env_commands(&env, bld);
  cpm_CLI(env);

  return 0;
}
