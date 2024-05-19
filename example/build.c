#include "cpm.h"
int main(int argc, char** argv){
  Arguments arg = {0};
  if(argc != 3){
    cpm_log(CPM_ERROR, "invalid arguments passed, argc: %d\n", argc);
    exit(-1);
  }
  if(cpm_strcmp("build", argv[1])){
    cpm_append_arguments(&arg, "build");
    cpm_submodule(argv[2], &arg);    
  }
  else if(cpm_strcmp("run", argv[1])){
    cpm_append_arguments(&arg, "build");
    cpm_submodule(argv[2], &arg);    
  }
  
  
}