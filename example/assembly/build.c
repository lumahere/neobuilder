#include "cpm.h"

#define buildpath "./build"

void build_objs(){
    Cmd cmd = {0};
    Cmd mainf = {0};
    cpm_cmd_append(&cmd, "gcc", "-c", "-fpic", "./awesomefunc.s", "-o" buildpath "/awesomefunc.o");
    cpm_cmd_append(&mainf, "gcc", "-c", "./main.c", "-o", buildpath "/main.o");
    cpm_compile(cmd);
    cpm_compile(mainf);
    
}

int main(void){
    cpm_mkdir(buildpath);
    build_objs();
    Cmd cmd = {0};
    cpm_cmd_append(&cmd, "gcc", "./build/awesomefunc.o", "./build/main.o", "-o", "main", "-static");
    cpm_compile(cmd);
    
}