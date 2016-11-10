#include <stdio.h>

#include "poker.h"
#include "printer.h"

const char *Hand::handName[16] = {
  "High card", "Pair", "Two pair", "Three card", "Straight", "Flush",
  "Full house", "Four card", "St. flush"
};

const char Hand::rankName[15] = "_A23456789TJQK";
const char Hand::suitName[] = "shdc";


int Hand::rankName2Number(char c) {
  if (c == 'A') return 1;
  if (c >= '1' && c <= '9') return c - '1' + 1;
  if (c == 'T') return 10;
  if (c == 'J') return 11;
  if (c == 'Q') return 12;
  if (c == 'K') return 13;
  return -1;
}

int Hand::suitName2Number(char c) {
  if (c == 's') return 0;
  if (c == 'h') return 1;
  if (c == 'd') return 2;
  if (c == 'c') return 3;
  return -1;
}

int Hand::str2cards(const char* in, Card* card, int len) {
  int n = 0, c;
  while ((c = *in++) && n < len) {
    if (c == ' ' || c == '-')
      continue;

    int rank = rankName2Number(c);
    if (rank < 0) {
      printf("rank error (%c)", c);
      return -1;
    }

    c = *in++;
    int suit = suitName2Number(c);
    if (suit < 0) {
      printf("suite error (%c)", c);
      return -1;
    }
    card[n++] = MAKE_CARD(suit, rank);
  }
  return n;
}


bool Hand::put(const char *in) {
  Card cards[7];
  int n = str2cards(in, cards, sizeof(cards) / sizeof(*cards));
  if (n < 0)
    return false;
  for(int i = 0; i < n; i++)
    put(cards[i]);
}

int Hand::calc() {
  int flush = -1;
  int flushKey = -1;
  int fourCard = -1;
  int threeCard = -1;
  int pair[2] = {-1, -1};
  int npair = 0;
  int kicker = -1;
  int straight = -1;
  bool straightFlush = false;

  for (int i = 0; i < 4; i++) {
    // Check flush
    if (color[i] >= 5) {
      flush = i;

      for (int j = 0; j < ncards; j++) {
        int rank = GET_NUMBER(cards[j]);
        rank = (rank == 1) ? 12 : rank - 2;
        if (GET_COLOR(cards[j]) == i && rank > flushKey) {
          flushKey = rank;
        }
      }
      break;
    }
  }

  for (int i = 13 - 1; i >= 0; i--) {
    // Check 2,3,4 cards
    if (number[i] >= 4) {
      fourCard = i;
      break;
    }

    if (threeCard == -1 && number[i] == 3) {
      threeCard = i;
    } else if (npair < 2 && number[i] >= 2) {
      pair[npair++] = i;
    }
  }

  // Check straight
  if (fourCard < 0 && !(threeCard >= 0 && npair > 0)) {
    // AKQJT-23456
    for (int i = 13 - 1; i >= 4; i--) {
      if (number[i - 0] > 0 &&
          number[i - 1] > 0 &&
          number[i - 2] > 0 &&
          number[i - 3] > 0 &&
          number[i - 4] > 0) {
        straight = i;
        break;
      }
    }

    if (straight < 0) {
      // A2345
      if (number[ 0] > 0 &&
          number[ 1] > 0 &&
          number[ 2] > 0 &&
          number[ 3] > 0 &&
          number[12] > 0) {
        straight = 3;
      }
    }
  }

  // Check straight flush
  if (straight >= 0 && (straight == flushKey || straight == 3 && flushKey == 12)) {
    int count = 0;
    for (int i = 0; i < ncards; i++) {
      if (GET_COLOR(cards[i]) != flush) continue;
      int rank = GET_NUMBER(cards[i]) - 2;
      if (rank <= straight && rank > straight - 5 || rank == -1 && straight == 12) {
        count++;
      }
    }
    if (count >= 5)
      straightFlush = true;
  }

  int ret;
  int numkicker = 0;
  int needsKicker = 0;

  if (straightFlush) {
    ret = MAKE_HAND(STRAIGHT_FLUSH, straight);
  } else if (fourCard >= 0) {
    ret = MAKE_HAND(FOUR_CARD, fourCard);
  } else if (threeCard >= 0 && npair > 0) {
    ret = MAKE_HAND(FULL_HOUSE, threeCard, pair[0]);
  } else if (flush >= 0) {
    ret = MAKE_HAND(FLUSH, flushKey);
  } else if (straight >= 0) {
    ret = MAKE_HAND(STRAIGHT, straight);
  } else if (threeCard >= 0) {
    ret = MAKE_HAND(THREE_CARD, threeCard);
    numkicker = 2;
    needsKicker = 2;
  } else if (npair == 2) {
    ret = MAKE_HAND(TWO_PAIR, pair[0], pair[1]);
    numkicker = 1;
    needsKicker = 1;
  } else if (npair == 1) {
    ret = MAKE_HAND(PAIR, pair[0]);
    numkicker = 3;
    needsKicker = 2;
  } else {
    ret = MAKE_HICARD();
    int n = 5;
    for (int i = 13 - 1; i >= 1 && n > 0; i--) {
      if (number[i]) {
        ret |= 1 << (i - 1);
        n--;
      }
    }
  }

  if (numkicker > 0) {
    int i = 13 - 1;
    do {
      if (number[i] == 1 ||
          (npair == 2 && number[i] == 2 && pair[0] != i && pair[1] != i)) {
        ret |= i << (needsKicker * 4);
        needsKicker--;
        numkicker--;
      }
      i--;
    } while (i >= 0 && numkicker > 0);
  }

  return ret;
}


#define READ_CHECK(addr, size) if (0 == fread(addr, size, sizeof(int), fp)) goto FAIL;

bool Result::dump(const char *fname) const {
  FILE *fp = fopen(fname, "wb");
  if (!fp) {
    printf("Cannot open %s\n", fname);
    return false;
  }

  fwrite(&nplayer, 1, sizeof(int), fp);
  fwrite(&nrepeat, 1, sizeof(int), fp);
  fwrite(&nstrategy, 1, sizeof(int), fp);
  fwrite(ncaller, nplayer + 1, sizeof(int), fp);

  for (int i = 0; i < nplayer; i++) {
    players[i].dump(fp);
  }
  fclose(fp);
  return true;
}

bool Result::restore(const char *fname) {
  FILE *fp = fopen(fname, "rb");
  if (!fp) {
    printf("Cannot open %s\n", fname);
    return false;
  }

  READ_CHECK(&nplayer, 1);
  READ_CHECK(&nrepeat, 1);
  READ_CHECK(&nstrategy, 1);
  init(nplayer, nstrategy);

  READ_CHECK(ncaller, nplayer + 1);

  for (int i = 0; i < nplayer; i++) {
    if (!players[i].restore(fp))
      goto FAIL;
  }

  fclose(fp);
  return true;

FAIL:
  fclose(fp);
  return false;
}

void PlayerResult::dump(FILE *fp) const {
  fwrite(&strategy, 1, sizeof(int), fp);

  fwrite(winnerHand, Hand::simplifiedHandSize, sizeof(int), fp);
  fwrite(allHand, Hand::simplifiedHandSize, sizeof(int), fp);
  fwrite(winnerHole, Hand::holeSize, sizeof(int), fp);
  fwrite(allHole, Hand::holeSize, sizeof(int), fp);
  fwrite(costByHole, Hand::holeSize, sizeof(int), fp);
  fwrite(incomeByHole, Hand::holeSize, sizeof(int), fp);
}

#undef READ_CHECK
#define READ_CHECK(addr, size) if (0 == fread(addr, size, sizeof(int), fp)) return false;

bool PlayerResult::restore(FILE *fp) {
  READ_CHECK(&strategy, 1);

  READ_CHECK(winnerHand, Hand::simplifiedHandSize);
  READ_CHECK(allHand, Hand::simplifiedHandSize);
  READ_CHECK(winnerHole, Hand::holeSize);
  READ_CHECK(allHole, Hand::holeSize);
  READ_CHECK(costByHole, Hand::holeSize);
  READ_CHECK(incomeByHole, Hand::holeSize);

  return true;
}

int PlayerResult::getIncome() const {
  int sum = 0;
  for (int i = 0; i < Hand::holeSize; i++) {
    sum += incomeByHole[i];
  }
  return sum;
}

int PlayerResult::getCost() const {
  int sum = 0;
  for (int i = 0; i < Hand::holeSize; i++) {
    sum += costByHole[i];
  }
  return sum;
}

#define ADD(addr) for (int i = 0; i < sizeof(addr) / sizeof(*addr); i++) addr[i] += a.addr[i];

PlayerResult& PlayerResult::operator+=(const PlayerResult& a) {
  strategy = a.strategy;
  ADD(winnerHole)
  ADD(allHole)
  ADD(costByHole)
  ADD(incomeByHole)
  ADD(winnerHand)
  ADD(allHand)
  return *this;
}

int Result::getStrategyResult(int strategy, PlayerResult *result) const {
  int n = 0;
  for (int j = 0; j < nplayer; j++) {
    if (players[j].strategy == strategy) {
      *result += players[j];
      n++;
    }
  }
  return n;
}


