# NEOBUILDER
#### *A standalone build system based on c*
NEOBUILDER (C-Project-Manager) is a standalone build system designed from c and, the entire build system is contained in a single header 
file "cpm.h" and is designed to be easy to use.

## Example
```c
    #include "cpm.h"

    int main(void){
        Cmd build = {0};
        cpm_cmd_append(&build, "cc");
        cpm_cmd_append(&build, "./src/main.c");
        cpm_cmd_append(&build, "-o", "main");
    } 

```
