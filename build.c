#include "cpm.h"

int main(int argc, char **argv) {
  CPM_REBUILD_SELF(argv);
  cpm_submodule("./example", 0);
}
