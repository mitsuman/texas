#include <stdio.h>
#include <stdlib.h>

#include "poker.h"
#include "printer.h"

void usage() {
  printf("dump [data file] (flag)\n");
}

int main(int argc, char **argv) {
  if (argc < 2) {
    usage();
    return -1;
  }

  int nopt = 1;

  Result result;
  if (!result.restore(argv[nopt])) {
    return -2;
  }
  nopt++;

  PrinterXterm printerXterm;
  PrinterHtml printerHtml;
  printer = &printerXterm;

  if (argc >= nopt + 1) {
    if (argv[nopt][0] == 'h') {
      printer = &printerHtml;
    }
    nopt++;
  }

  result.show();

  return 0;
}
