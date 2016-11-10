#include <stdio.h>
#include <stdlib.h>

#include "poker.h"

void usage() {
  printf("dat2range [data file]\n");
}

static int cmpr_int(const void*a, const void*b) {
  int x =*((int*)a); 
  int y =*((int*)b);
  return x < y ? 1 : x > y ? -1 : 0;
}

int main(int argc, char **argv) {
  if (argc < 1 + 1) {
    usage();
    return -1;
  }

  int nopt = 1;

  Result result;
  if (!result.restore(argv[nopt])) {
    return -2;
  }
  nopt++;

  PlayerResult pres;
  if (result.getStrategyResult(0, &pres) == 0) {
    printf("Error, No strategy.\n");
    return -1;
  }

  int win[Hand::holeSize];
  for (int i = 0; i < Hand::holeSize; i++) {
    if (pres.costByHole[i] == 0) {
      printf("Error, zero div.\n");
      return -1;
    }

    int ratio = pres.costByHole[i] ? (int) (1.0f * 0xff * pres.incomeByHole[i] / pres.costByHole[i]) : 0;

    win[i] = (ratio << 16) | i;
  }

  qsort(win, sizeof(win) / sizeof(*win), sizeof(*win), cmpr_int);

  int order[Hand::holeSize];
  int sum = 0;
  for (int i = 0; i < Hand::holeSize; i++) {
    int h = win[i] & 0xff;
    if (h >= Hand::holeSize) {
      printf("inside error\n");
      return -1;
    }

    sum += pres.allHole[h];
    order[h] = sum;
  }
  printf("\n");

  for (int _j = 14; _j >= 2; _j--) {
    int j =  (_j == 14) ? 1 : _j;
    printf("%c ", Hand::rankName[j]);

    for (int _i = 14; _i >= 2; _i--) {
      int i =  (_i == 14) ? 1 : _i;

      int o = order[(_i >= _j) ? GET_HOLE(i, j, false) : GET_HOLE(j, i, true)];
      printf("%4d ", (int) (1000.0f * o / sum));
    }
    printf("\n");
  }

  return 0;
}
