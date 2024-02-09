#include "cpm.h"

int main(int argc, char** argv){
    CPM_REBUILD_SELF(argv);
// Do you love me? OwO
    const char* src = "./main.c";
    const char* exec = "./main";
    Cmd cmd = {0};
    cpm_cmd_append(&cmd, "clang", src, "-o", exec);
    cpm_recompile(src, exec, cmd);
}