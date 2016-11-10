#include <ctype.h>
#include <stdio.h>

#include "strategy.h"
#include "table.h"

Table::Table() {
  strategies = 0;
  players = 0;
  debug = !true;
}

Table::~Table() {
  destroy();
}

void Table::destroy() {
  delete strategies;
  strategies = 0;

  delete players;
}

bool Table::loadStrategy(int n, const char *fname) {
  if (n < 0 || n >= nstrategy) {
    return false;
  }
  Strategy *s = Strategy::load(fname);
  if (!s) {
    return false;
  }
  strategies[n] = s;
  return true;
}

bool Table::setStrategy(int a, int b) {
  if (a < 0 || a >= nplayer) {
    return false;
  }
  if (b < 0 || b >= nstrategy) {
    return false;
  }
  players[a] = b;
  return true;
}

void Table::prepare() {
  hands = new Hand[nplayer];
  actions = new Action[nplayer];
  winners = new int[nplayer];

  utg = 0;
  BB = 10;
  results.init(nplayer, nstrategy);

  for (int i = 0; i < nplayer; i++) {
    results.players[i].strategy = players[i];
  }
}

bool Table::loadConfig(const char *fname) {
  destroy();

  nplayer = 0;
  nstrategy = 0;

  FILE *fp = fopen(fname, "rt");
  if (!fp) {
    printf("Cannot load %s\n", fname);
    return false;
  }

  int lineno = 0;

  while (feof(fp) == 0) {
    char line[256];
    if (0 == fgets(line, sizeof line, fp)) {
      break;
    }

    lineno++;

    if (isspace(*line) || *line == '#') {
      continue;
    }

    int n, a, b;
    char fname[256];

    if (1 == sscanf(line, "player-count %d", &n)) {
      initPlayer(n);
    } else if (1 == sscanf(line, "strategy-count %d", &n)) {
      initStrategy(n);

    } else if (2 == sscanf(line, "load-strategy %d %s", &n, fname)) {
      if (!loadStrategy(n, fname)) {
        printf("%d:Failed to load strategy %s to %d\n", lineno, fname, n);
        return false;
      }

    } else if (2 == sscanf(line, "set-strategy %d %d", &a, &b)) {
      if (!setStrategy(a, b)) {
        printf("%d:Failed to set strategy number(%d) to player(%d).\n", lineno, b, a);
        return false;
      }

    } else {
      printf("%d:unrecognized line.\n", lineno);
      return false;
    }
  }

  if (nplayer == 0) {
    printf("%d:Set player-count.\n", lineno);
    return false;
  }

  prepare();

  fclose(fp);
  return true;
}

void Table::serveHole() {
  for (int k = 0; k < 2; k++) {
    for (int j = 0; j < nplayer; j++) {
      hands[j].put(deck.pull());
    }
  }

  if (debug) {
    for (int _j = 0; _j < nplayer; _j++) {
      int j = (_j + utg) % nplayer; // player number
      printf("%2d:%1d ", j, players[j]);
    }
    printf("\n");

    for (int _j = 0; _j < nplayer; _j++) {
      int j = (_j + utg) % nplayer; // player number
      hands[j].print();
      printf(" ");
    }
    printf("\n");
  }
}

void Table::serveFlop() {
  serveCommunityCard(3);
}

void Table::serveTurn() {
  serveCommunityCard(1);
}

void Table::serveRiver() {
  serveCommunityCard(1);

  if (debug) {
    printf("\n");
  }
}

void Table::serveCommunityCard(int n) {
  for (int k = 0; k < n; k++) {
    Card c = deck.pull();
    if (debug) {
      Hand::printCard(c);
    }
    for (int j = 0; j < nplayer; j++) {
      hands[j].put(c);
    }
  }
}

void Table::calc() {
  assert(players);
  assert(strategies);
  assert(hands);

  deck.shuffle();

  for (int j = 0; j < nplayer; j++) {
    hands[j].clear();
  }

  nrestPlayer = nplayer;
  pot = 0;

  // Serve hole card
  serveHole();
  betting();

  // Serve community card (flop/turn/river)
  serveFlop();
  serveTurn();
  serveRiver();

  detectWinner();

  utg++;
  results.nrepeat++;
}

void Table::detectWinner() {
  // Detect winner
  int highhand = -1;
  int winner = (utg + nplayer - 1) % nplayer; // BB is winner if noone bets.
  int nwinner = 0;

  int ncall = 0;

  for (int j = 0; j < nplayer; j++) {
    PlayerResult* r = &results.players[j];
    r->allHole[hands[j].getHole()]++;

    if (actions[j] == Fold) {
      continue;
    }

    ncall++;

    int h = hands[j].calc();
    if (h == highhand) {
      winners[nwinner] = j;
      nwinner++;
    } else if (h > highhand) {
      highhand = h;
      nwinner = 1;
      winners[0] = j;
    }

    assert(0 <= CNV_SIMPLIFIED_HAND(h) &&
           CNV_SIMPLIFIED_HAND(h) < Hand::simplifiedHandSize);
    r->allHand[CNV_SIMPLIFIED_HAND(h)]++;
  }

  assert(0 < highhand && highhand <= MAKE_HAND(STRAIGHT_FLUSH, 12));

  record(nwinner, highhand);

  results.ncaller[ncall]++;
}

void Table::record(int nwinner, int highHand) {
  int prize = pot / nwinner;
  for (int i = 0; i < nwinner; i++) {
    int j = winners[i];
    PlayerResult* r = &results.players[j];
    int hole = hands[j].getHole();

    assert(hole >= 0 && hole < sizeof(results.players[j].winnerHole) / sizeof(*results.players[j].winnerHole));

    r->winnerHand[CNV_SIMPLIFIED_HAND(highHand)] ++;
    r->winnerHole[hole] ++;
    r->incomeByHole[hole] += prize;
  }

  if (debug) {
    for (int _j = 0; _j < nplayer; _j++) {
      int j = (_j + utg) % nplayer; // player number

      bool iswinner = false;
      for (int k = 0; k < nwinner; k++) {
        if (winners[k] == j) {
          iswinner = true;
          break;
        }
      }

      printf("%c    ", iswinner ? '*' : ' ');
    }
    printf("\n");
  }
}

void Table::betting() {
  for (int _j = 0; _j < nplayer; _j++) {
    actions[_j] = Unknown;
  }

  nraiser = 0;
  bet = BB;
  lastRaiser = nplayer - 1; // BB
  int loop = 0;

  for (int _j = 0;;) {
    if (nrestPlayer == 1) {
      break;
    }

    if (_j == lastRaiser && loop > 0)
      break;

    int j = (_j + utg) % nplayer; // player number

    Action act = Fold;
    if (actions[j] != Fold) {
      order = _j;
      act = strategies[players[j]]->calc(hands + j, this);

      actions[j] = act;

      // Update pot
      int mybet = 0;
      if (act >= Raise) {
        mybet = bet = bet * (2 + (act - Raise));
        nraiser++;
        lastRaiser = _j;
      } else if (act == Call) {
        mybet = bet;
      } else if (act == Fold) {
        nrestPlayer--;
        if (loop == 0) {
          // BB should be post BB
          if (_j == nplayer - 1) {
            mybet = BB;
          }
          // SB should be post SB
          if (_j == nplayer - 2) {
            mybet = BB / 2;
          }
        }
      } else {
        assert(false);
      }

      results.players[j].costByHole[hands[j].getHole()] += mybet;
      pot += mybet;

      if (debug) {
        if (act == Fold) {
          printf("F    ");
        } else if (act == Call) {
          printf("C    ");
        } else {
          printf("%3d  ", mybet);
        }
      }
    } else {
      if (debug) {
        printf("     ");
      }
    }

    if (act < Raise && _j == lastRaiser)
      break;

    _j++;

    if (_j >= nplayer) {
      _j = 0;
      loop++;

      if (debug) {
        printf("\n");
      }
    }
  }

  if (debug) {
    printf("\n");
  }
}

bool Table::dump(const char *fname) {
  return results.dump(fname);
}
