#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "poker.h"
#include "printer.h"

Printer *printer = 0;

void rankHeader(int space = 5) {
  const char str[] = "          ";
  const char *s = str + sizeof(str) - space - 1;
  printf("%s", s);
  for (int _i = 14; _i >= 2; _i--) {
    int i =  (_i == 14) ? 1 : _i;
    printf("%c%s", Hand::rankName[i], s + 1);
  }
  printf("\n");
}

void statisticsHole(const int *costByHole, const int *incomeByHole, bool percent = false, float mul = 1, float thr = 1) {
  rankHeader();

  for (int _j = 14; _j >= 2; _j--) {
    int j =  (_j == 14) ? 1 : _j;
    printf("%c ", Hand::rankName[j]);

    for (int _i = 14; _i >= 2; _i--) {
      int i =  (_i == 14) ? 1 : _i;

      if (_i >= _j)
        printer->odds(incomeByHole[GET_HOLE(i, j, false)], costByHole[GET_HOLE(i, j, false)], i, j, percent, mul, thr);
      else
        printer->odds(incomeByHole[GET_HOLE(j, i, true)], costByHole[GET_HOLE(j, i, true)], j, i, percent, mul, thr);
      printf(" ");
    }

    printf("%c\n", Hand::rankName[j]);
  }
  rankHeader();
}

void PrinterXterm::odds(int income, int cost, int x, int y, bool percent, float mul, float thr) {
  const char *format = percent ? "%4.1f" : "%4.2f";
  float val = cost > 0 ? 1.0f * income / cost * mul : -1;

  if (x == y) {
    printf("\e[4m");
  }

  int gray = (int) (log10(val / thr) * 32 + 23 / 2);
  if (gray < 2) gray = 5;
  if (gray > 23) gray = 23;

  printf("\e[38;5;%dm", 232 + gray);
  if (val > 99.9f)
    printf("100.");
  else if (cost > 0)
    printf(format, val);
  else
    printf(percent ? "--.-" : "-.--");

  printf("\e[0m");
}

void PrinterHtml::header() {
  printf("<html><head><style>"
         "@import url(http://fonts.googleapis.com/css?family=Source+Code+Pro:600);\n"
         "</style><body style='margin:0px;'><pre style='color:#ccc;background:#000;padding:2px;margin:0px;font:12px \"Source Code Pro\",monospace;overflow-x:hidden'>");
}

void PrinterHtml::footer() {
  printf("</body></html>");
}

void PrinterHtml::odds(int income, int cost, int x, int y, bool percent, float mul, float thr) {
  const char *format = percent ? "%4.1f" : "%3.2";
  float val = cost > 0 ? 1.0f * income / cost * mul : -1;

  int gray = (int) ((val / thr) * 128);
  if (gray < 50) gray = 50;
  if (gray > 255) gray = 255;

  printf("<span style='color:#%02x%02x%02x;%s'>", gray, gray, gray, x == y ? "text-decoration:underline" : "");
  if (val > 99.9f)
    printf("100.");
  else if (cost > 0)
    printf(format, val);
  else
    printf(percent ? "--.-" : "-.--");

  printf("</span>");
}

// --------------------------------
void PlayerResult::showWinningHands(int nrepeat, int nplayer) const {
  printf("%10s        Winning/      Showdown/ Winratio\n", "");
  for (int i = 0; i < HAND_MAX; i++) {
    int win = 0, all = 0;
    for (int j = 0; j < 13; j++) {
      win += winnerHand[i * CARD_MAX + j];
      all += allHand[i * CARD_MAX + j];
    }
    printf("%10s %8d %4.1f%% %8d %4.1f%%    %5.1f%%\n",
           Hand::handName[i],
           win, 100.0f * win / nrepeat,
           all, 100.0f * all / nrepeat / nplayer,
           100.0f * win / all);
  }
}

void PlayerResult::showWinningRatioEachHands(int nplayer) const {
  printf("%9s", ""); rankHeader(5);
  for (int i = 0; i < HAND_MAX; i++) {
    printf("%10s ", Hand::handName[i]);
    for (int j_ = 14; j_ >= 2; j_--) {
      int j = j_ == 14 ? 0 : j_ - 1;
      printer->odds(winnerHand[i * CARD_MAX + j], allHand[i * CARD_MAX + j],
                0, 1, true, 100.0f, 100.0f / nplayer);
      printf(" ");
    }
    printf("\n");
  }
  printf("%9s", ""); rankHeader(5);
}

void PlayerResult::showRatioOfWinningHands() const {
  int sumWinHand = 0;
  for (int i = 0; i < HAND_MAX; i++) {
    for (int j = 0; j < 13; j++) {
      sumWinHand += winnerHand[i * CARD_MAX + j];
    }
  }

  printf("%9s", ""); rankHeader(5);
  int sum = 0;
  for (int i = 0; i < HAND_MAX; i++) {
    printf("%10s ", Hand::handName[i]);
    int a[13];
    for (int j_ = 2; j_ <= 14; j_++) {
      int j = j_ == 14 ? 0 : j_ - 1;
      int k = winnerHand[i * CARD_MAX + j];
      sum += k;
      if (k)
        a[j] = sum;
      else
        a[j] = -1;
    }

    for (int j_ = 14; j_ >= 2; j_--) {
      int j = j_ == 14 ? 0 : j_ - 1;
      printer->odds(a[j], a[j] > 0 ? sumWinHand : 0,
                    0, 1, true, 100.0f, 100.0f / 2);
      printf(" ");
    }
    printf("\n");
  }
  printf("%9s", ""); rankHeader(5);
}

void PlayerResult::showWinningRatioByHole(int nplayer) const {
  statisticsHole(allHole, winnerHole, true, 100, 100.0 / nplayer);
}

void PlayerResult::showEVByHole() const {
  statisticsHole(costByHole, incomeByHole);
}

void PlayerResult::showEVPercentByHole() const {
  statisticsHole(costByHole, incomeByHole, true, 100 / 2, 100.0 / 2);
}

void Result::showInThePotPlayers() const {
  if (ncaller[nplayer] != nrepeat) {
    printf("In-the-pot players\n");
    for (int i = 1; i <= nplayer; i++) {
      printf("%2d:%6.2f%%\n", i, 100.0f * ncaller[i] / nrepeat);
    }
    printf("\n");
  }
}

void PlayerResult::show(int nplayer, int nrepeat) const {
  printf("Winning ratio of each hand\n");
  showWinningRatioEachHands(nplayer);
  printf("\n");

  printf("Ratio of winning hand\n");
  showRatioOfWinningHands();
  printf("\n");

  printf("Winning ratio by Hole\n");
  showWinningRatioByHole(nplayer);
  printf("\n");

  printf("EV by Hole\n");
  showEVByHole();
  printf("\n");
}

void Result::show() const {
  printf("Simulation results (%d players, %d games)\n", nplayer, nrepeat);

#if 0
  printf("Result each player");
  for (int i = 0; i < nplayer; i++) {
    players[i].show(nplayer, nrepeat);
  }
#endif

  printf("Result each strategy\n");
  for (int i = 0; i < nstrategy; i++) {
    PlayerResult r;
    int n = getStrategyResult(i, &r);
    printf("Strategy:%d (%d)\n", i, n);
    r.show(n, nrepeat);
  }

  printf("Odds each player\n");
  for (int i = 0; i < nplayer; i++) {
    int income = players[i].getIncome();
    int cost = players[i].getCost();

    printf("%d:%d: ", i, players[i].strategy);
    if (cost == 0) {
      printf("\n");
    } else {
      printf("%7d/%7d %5.3f\n",
             income, cost,
             1.0f * income / cost);
    }
  }
}
