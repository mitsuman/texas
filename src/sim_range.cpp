#include <stdio.h>
#include <algorithm>
#include "mtseq.h"

__NS_MTSEQ::MTSequence mt;

const int MAX_RANGE = 0x7fff;
const int PLAYER = 9;
const int ST_MAX = PLAYER * PLAYER;

class St {
public:
  int r, c;
  int cost, profit;

  St() : r(MAX_RANGE / 2), c(MAX_RANGE / 2), cost(1), profit(0) {}

  bool operator <(const St& a) const {
    return a.profit * cost > profit * a.cost;
  }

  void swing(int dev) {
    int a = mt.genrand_int31();
    r += a % (dev * 2 + 1) - dev;
    a /= (dev * 2 + 1);
    c += a % (dev * 2 + 1) - dev;

    if (r < 0) r = 0;
    if (r > MAX_RANGE) r = MAX_RANGE;
    if (c < 0) c = 0;
    if (c > MAX_RANGE) c = MAX_RANGE;
  }
};

St* getSt(St* st, int pos, int raiser) {
  return &st[pos * PLAYER + raiser];
  //return &st[pos];
}

void sim(St* mst, int np, int* initCost) {
  int s[PLAYER];
  St* sst[PLAYER];
  bool raised = false;
  int raiser = 0;

  int winner = -1, winnerh = MAX_RANGE;
  int pot = 0;

  for (int i = np - 1; i >= 0; i--) {
    St* st = getSt(mst, i, raiser);
    sst[i] = st;

    int h = mt.genrand_int31() & MAX_RANGE;

    if (raised) {
      if (h > st->c) {
        st->cost += initCost[i];
        pot      += initCost[i];
        continue;
      }
    } else if (np > 0) {
      if (h > st->r) {
        st->cost += initCost[i];
        pot      += initCost[i];
        continue;
      }
      raiser = i;
    }

    st->cost += 2;
    pot      += 2;

    raised = true;
    if (winnerh > h) {
      winnerh = h;
      winner = i;
    }
  }

  sst[winner]->profit += pot;
}

bool learn(St* mst, int np, int *initCost) {
  St pst[ST_MAX];
  for (int i = 0; i < ST_MAX; i++) {
    pst[i] = mst[i];
    pst[i].swing(0xff);

    pst[i].cost = pst[i].profit = 0;
    mst[i].cost = mst[i].profit = 0;
  }

  for (int i = 0; i < 80000; i++) {
    sim(mst, np, initCost);
    sim(pst, np, initCost);
  }

  bool updated = false;
  for (int i = 0; i < ST_MAX; i++) {
    if (pst[i] < mst[i])
      continue;
    mst[i] = pst[i];
    updated = true;
  }
  return updated;
}

void dump(St* mst) {
#if 0
  for (int i = 0; i < PLAYER; i++) {
    St* s = getSt(mst, i, 0);
    printf("%03d,%03d %4.2f ", s->r * 999 / MAX_RANGE, s->c * 999 / MAX_RANGE, s->cost ? (float)s->profit / s->cost : 0);
  }
  printf("\n");
#endif
  for (int i = 0; i < PLAYER; i++) {
    printf("%d:", i);
    for (int j = 0; j < PLAYER; j++) {
      //int j_ = (j + PLAYER - 1) % PLAYER;
      //if (i < j) break;
      St* s = getSt(mst, j, i);
      printf("%03d,%03d %4.2f ", s->r * 999 / MAX_RANGE, s->c * 999 / MAX_RANGE, s->cost ? (float)s->profit / s->cost : 0);
    }
    printf("\n");
  }
  printf("\n");
}

int main() {
  St st[ST_MAX];

  int initCost[PLAYER];
  for (int i = 0; i < PLAYER; i++)
    initCost[i] = 1;
  initCost[0] = 10;
  initCost[1] = 5;

  for (int i = 0; i < 1000; i++) {
    if (learn(st, PLAYER, initCost))
      dump(st);
  }

  return 0;
}
