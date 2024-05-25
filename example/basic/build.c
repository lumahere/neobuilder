#include "cpm.h"

int main(){
    Cmd build = {0};
    cpm_cmd_append(&build, "cc");
    cpm_cmd_append(&build, "hello.c");
    cpm_cmd_append(&build, "-Wall", "-Wextra");
    cpm_cmd_append(&build, "-o");
    cpm_cmd_append(&build, "hello");

    cpm_compile(build);
    return 0;
}