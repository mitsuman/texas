#include <stdio.h>
#include <stdlib.h>

#include "poker.h"
#include "strategy.h"
#include "table.h"

void usage() {
  printf("sim [table config] [repeat number] [output file]\n");
}

// --------------------------------

int main(int argc, char **argv) {
  if (argc < 1 + 3) {
    usage();
    return -1;
  }

  Table table;
  if (!table.loadConfig(argv[1])) {
    printf("Failed to load %s\n", argv[1]);
    return -1;
  }

  int nrepeat = atoi(argv[2]);
  if (nrepeat < 1) {
    usage();
    return -1;
  }

  const char *foutput = argv[3];

  for (int i = 0; i < nrepeat; i++) {
    table.calc();
  }

  table.dump(foutput);
}
