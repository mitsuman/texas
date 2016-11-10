#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "poker.h"
#include "strategy.h"
#include "table.h"
#include "printer.h"

void usage() {
  printf("sim_hu [hole(-flip)(-turn)(-river)] [repeat number] [{text,html}(-{ev,nolose})]\n");
}

// --------------------------------

class TableHu : public Table {
public:
  TableHu() {
    Table();
    initPlayer(2);
    initStrategy(1);
    loadStrategy(0, "calling-station");
    setStrategy(0, 0);
    setStrategy(1, 0);
    prepare();
  }

  bool setPreset(Card *cards, int ncard) {
    preset = cards;
    npreset = ncard;

    for (int i = 0; i < npreset; i++) {
      if (!deck.remove(preset[i])) {
        printf("Double count at %d\n", i + 1);
        return false;
      }
    }
    return true;
  }

private:
  void serveHole() {
    for (int k = 0; k < 2; k++) {
      hands[0].put(preset[k]);
      hands[1].put(deck.pull());
    }
  }

  void serveFlop() {
    serveCommunityCard(3, 2);
  }

  void serveTurn() {
    serveCommunityCard(1, 5);
  }

  void serveRiver() {
    serveCommunityCard(1, 6);
  }

  void serveCommunityCard(int n, int offset) {
    for (int k = 0; k < n; k++) {
      Card c;
      if (npreset > offset)
        c = preset[offset + k];
      else
        c = deck.pull();

      for (int j = 0; j < 2; j++)
        hands[j].put(c);
    }
  }

  void record(int nwinner, int highHand) {
    bool won = false;
    for (int i = 0; i < nwinner; i++) {
      if (winners[i] == 0) {
        won = true;
        break;
      }
    }

    if (won) {
      int hole = hands[1].getHole();
      results.players[1].incomeByHole[hole] += pot / nwinner;
      results.players[1].winnerHole[hole] ++;
    }
  }

  Card* preset;
  int npreset;
};

int main(int argc, char **argv) {
  if (argc < 1 + 2) {
    usage();
    return -1;
  }

  Card cards[7];
  int n = Hand::str2cards(argv[1], cards, sizeof(cards) / sizeof(*cards));
  if (n < 2)
    return -1;

  TableHu table;
  if (!table.setPreset(cards, n)) {
    return -1;
  }

  int nrepeat = atoi(argv[2]);
  if (nrepeat < 1) {
    usage();
    return -1;
  }

  for (int i = 0; i < nrepeat; i++) {
    table.calc();
  }

  PrinterXterm printerXterm; printer = &printerXterm;
  PrinterHtml printerHtml;

  bool nolose = false;

  if (argc >= 1 + 3) {
    if (strstr(argv[3], "html") != 0) {
      printer = &printerHtml;
    }
    if (strstr(argv[3], "nolose") != 0) {
      nolose = true;
    }
  }


  printer->header();
  printf("%s (%d sampling)\n", argv[1], nrepeat);
  if (nolose) {
    table.results.players[1].showWinningRatioByHole(2);
  } else {
    table.results.players[1].showEVByHole();
  }
  printer->footer();
  return 0;
}
