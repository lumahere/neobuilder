#include "cpm.h"

int main(int argc, char **argv) {
  switch (cpm_input_check(argc, argv)) {
  case BUILD:
    cpm_submodule("./example", "build");
    break;
  case RUN:
    cpm_submodule("./example", "run");
    break;
  case CLEAN:
    cpm_submodule("./example", "clean");
    break;
  };
  return 0;
}
