#include "cpm.h"

int main(int argc, char **argv) {
  CPM_REBUILD_SELF(argv);
  cpm_submodule("./submod1");
  cpm_submodule("./submod2");
}
