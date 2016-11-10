#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "strategy.h"

static const char *getWord(const char *p, char *out) {
  while (isspace(*p)) p++;
  if (*p == 0) return 0;

  while (!isspace(*p)) {
    *out++ = *p++;
    if (*p == 0)
      break;
  }
  *out = 0;
  return p;
}

class CallingStationStrategy : public Strategy {
protected:
  Action calc(Hand *hand, TableInfo* dat) {
    return Call;
  }
};

class PreFlopStrategy : public Strategy {
public:
  PreFlopStrategy() : actions(0), nplayer(0), raiseSize(0) {}
  ~PreFlopStrategy() { destroy(); }

private:
  void destroy() {
    delete actions;
    actions = 0;
  }

  Action calc(Hand *hand, TableInfo* tbl) {
    int hole = hand->getHole();
    int nraiser = tbl->nraiser;
    if (nraiser >= RAISER_MAX) {
      return Call;
    }
    int rank = holeRank[hole];
    if (hole == 0) {
      assert(rank == 4);
    }

    int order = tbl->order + nplayer - tbl->nplayer;
    int raiserank = actions[order][ACT_RAISE][nraiser];
    int callrank = actions[order][ACT_CALL][nraiser];

#if 0 // debug
    printf("%d,%d,%d ", rank, callrank, raiserank);
#endif

    if (rank <= raiserank) {
      return Raise + (raiseSize - 2);
    }
    if (rank <= callrank) {
      return Call;
    }
    return Fold;
  }

  bool loadHoleRank(const char* fname);
  bool processConfigLine(const char* line);

  bool processConfigFinish() {
    if (nplayer <= 0)
      return false;

    return true;
  }

private:
  const static int RAISER_MAX = 4;
  int holeRank[Hand::holeSize]; //  (strong) 0..1000 (weak)

  int nplayer;

  const static int ACT_CALL = 0, ACT_RAISE = 1, ACTION_MAX = 2;
  // [Call=0, Raise=1] [state of raiser] [position]
  int (*actions)[ACTION_MAX][RAISER_MAX];

  int raiseSize; // 0=x2, 1=x3, 2=x4, ...

  // load config
  enum ConfigState {
    LOAD_RANK,
    SET_RANGE
  };

  ConfigState configState;
};

static int cmpr_int(const void*a, const void*b) {
  return *((int*)a) > *((int*)b);
}

bool PreFlopStrategy::processConfigLine(const char* line) {
  int position;
  char fname[256];
  char word[256];
  const char *p;
  *word = 0;
  p = getWord(line, word);

  if (0 == strcmp(word, "load-rank")) {
    configState = LOAD_RANK;

  } else if (0 == strcmp(word, "set-range")) {
    configState = SET_RANGE;

  } else if (1 == sscanf(line, "set-raise-size %d", &raiseSize)) {
    if (raiseSize < 2) {
      printf("invalid raise-size, it should be 2 or lager. 2 means minimum raise size, and n means current bet size x n.\n");
      return false;
    }
  } else if (1 == sscanf(line, "player-count %d", &nplayer)) {
    delete actions;
    actions = new int[nplayer][ACTION_MAX][RAISER_MAX];

  } else if (configState == LOAD_RANK) {
    int n = Hand::rankName2Number(word[0]);
    if (n <= 0 || n > 13) {
      printf("invalid rank '%c' (%d)\n", word[0], n);
      return false;
    }
    int n_ = n == 1 ? 14 : n;

    for (int i_ = 14; i_ >= 2; i_--) {
      int i = i_ == 14 ? 1 : i_;
      int hole = n_ > i_ ? GET_HOLE(n, i, true) : GET_HOLE(i, n, false);
      if (hole < 0 || hole >= Hand::holeSize) {
        printf("invalid hole %d (%d, %d)\n", hole, n, i);
        return false;
      }

      p = getWord(p, word);
      if (!p) {
        return false;
      }
      int rank = atoi(word);;
      if (rank == 0) {
        return false;
      }
      holeRank[hole] = rank;
    }

  } else if (configState == SET_RANGE) {
    int position = atoi(word);
    if (position < 0 || position >= nplayer) {
      return false;
    }

    for (int r = 0; r < RAISER_MAX; r++) {
      p = getWord(p, word);
      if (!p)
        return false;

      int call, raise;
      if (2 != sscanf(word, "%d,%d", &call, &raise))
        return false;

      if (call < 0 || call > 1000 || raise < 0 || raise > 1000)
        return false;

      actions[position][ACT_CALL ][r] = call;
      actions[position][ACT_RAISE][r] = raise;
    }
  }

  return true;
}

Strategy* Strategy::load(const char* fname) {
  if (0 == strcmp(fname, "calling-station")) {
    return new CallingStationStrategy;
  }

  FILE *fp = fopen(fname, "rt");
  if (!fp) {
    printf("Cannot open %s\n", fname);
    return 0;
  }

  Strategy *strategy = new PreFlopStrategy();

  char line[256];
  char datafname[128];
  int lineno = 1;

  while (fgets(line, sizeof line, fp)) {
    if (strncmp(line, "eof", 3) == 0) {
      break;
    }
    if (line[0] == '#' || isspace(line[0]))
      continue;
    if (!strategy->processConfigLine(line)) {
      printf("%d:error\n", lineno);
      return 0;
    }
    lineno++;
  }

  if (!strategy->processConfigFinish()) {
    printf("Strategy initialize error.\n");
    return 0;
  }

  fclose(fp);

  return strategy;
}
