#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "poker.h"
#include "strategy.h"

void usage() {
  printf("allcall [player number] [repeat number] [output file] (hole strategy file) (flag)\n");
}

// --------------------------------

Action strategyAlwaysCall(Hand *hand, StrategyData*) {
  return Call;
}

// --------------------------------

int main(int argc, char **argv) {
  if (argc < 4) {
    usage();
    return -1;
  }

  int nplayer = atoi(argv[1]);
  if (nplayer < 1) {
    usage();
    return -1;
  }

  int nrepeat = atoi(argv[2]);
  if (nrepeat < 1) {
    usage();
    return -1;
  }

  const char *foutput = argv[3];

  // @return call if 0, raise if plus, fold if minus
  Action (*strategy)(Hand *,StrategyData*) = strategyAlwaysCall;

  Deck deck;
  Result result(nplayer, 1);
  result.nrepeat = nrepeat;

  {
    Hand *hands = new Hand[nplayer];
    Action *actions = new Action[nplayer];

    StrategyData strategyData;
    strategyData.nplayer = nplayer;
    strategyData.actions = actions;

    int utg = 0;
    const int SB = 1;
    const int BB = 2;

    for (int i = 0; i < nrepeat; i++) {
      deck.shuffle();

      for (int j = 0; j < nplayer; j++) {
        hands[j].clear();
      }

      // Serve hole card
      for (int k = 0; k < 2; k++) {
        for (int j = 0; j < nplayer; j++) {
          hands[j].put(deck.pull());
        }
      }

      //
      strategyData.pot = 0;

      // Preflop strategy

      // bet time
      for (int _j = 0; _j < nplayer; _j++) {
        strategyData.actions[_j] = Unknown;
      }

      strategyData.nraiser = 0;
      strategyData.bet = BB;
      strategyData.lastRaiser = nplayer - 1;
      int loop = 0;
      int nrestPlayer = nplayer;

      for (int _j = 0;;) {
        int j = (_j + utg) % nplayer;

        if (nrestPlayer == 1)
          break;

        Action act = Fold;
        if (actions[_j] != Fold) {
          strategyData.order = _j;
          act = strategy(hands + j, &strategyData);

          actions[_j] = act;

          // Update pot
          int bet = 0;
          if (act >= Raise) {
            bet = strategyData.bet = strategyData.bet * 2 + (act - Raise);
            strategyData.nraiser++;
            strategyData.lastRaiser = _j;
          } else if (act == Call) {
            bet = strategyData.bet;
          } else if (act == Fold) {
            nrestPlayer--;
            if (loop == 0) {
              // BB should be post BB
              if (_j == nplayer - 1) {
                bet = BB;
              }
              // SB should be post SB
              if (_j == nplayer - 2) {
                bet = SB;
              }
            }
          }

          result.players[0].costByHole[hands[j].getHole()] += bet;
          strategyData.pot += bet;
        }

        if (act != Raise && _j == strategyData.lastRaiser)
          break;

        _j++;
        if (_j >= nplayer) {
          _j = 0;
          loop++;
        }
      }

      // Serve community card (flop/turn/river)
      for (int k = 0; k < 5; k++) {
        Card c = deck.pull();
        for (int j = 0; j < nplayer; j++) {
          hands[j].put(c);
        }
      }

      // Detect winner
      int highhand = -1;
      int winner = (utg + nplayer - 1) % nplayer; // BB is winner if noone bets.
      int nwinner = 0;

      int ncall = 0;

      for (int j = 0; j < nplayer; j++) {
        result.players[0].allHole[hands[j].getHole()]++;

        if (actions[j] == Fold) {
          continue;
        }

        ncall++;

        int h = hands[j].calc();
        if (h == highhand) {
          nwinner++;
        } else if (h > highhand) {
          highhand = h;
          winner = j;
          nwinner = 1;
        }

        assert(0 <= CNV_SIMPLIFIED_HAND(h) &&
               CNV_SIMPLIFIED_HAND(h) < Hand::simplifiedHandSize);
        result.players[0].allHand[CNV_SIMPLIFIED_HAND(h)]++;
      }

      assert(0 < highhand && highhand <= MAKE_HAND(Hand::STRAIGHT_FLUSH, 12));
      result.players[0].winnerHand[CNV_SIMPLIFIED_HAND(highhand)] += nwinner;

      int hole = hands[winner].getHole();
      assert(hole >= 0 && sizeof(result.players[0].winnerHole) / sizeof(*result.players[0].winnerHole));
      result.players[0].winnerHole[hole] += nwinner;
      result.players[0].incomeByHole[hole] += strategyData.pot;

      result.ncaller[ncall]++;

      utg++;
    }

    delete []hands;
    delete []actions;
  }

  result.dump(foutput);
}
