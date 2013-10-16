#include "poker.h"

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
    if (color[i] >= 4) {
      flush = i;

      for (int j = 0; j < ncards; j++) {
        int rank = CNV_ACE(GET_NUMBER(cards[j]));
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
    } else if (npair < 2) {
      pair[npair++] = i;
    }
  }

  // Check straight
  if (!fourCard && !threeCard && npair < 2) {
    // Case of AKQJT
    if (number[ 0] > 0 &&
        number[12] > 0 &&
        number[11] > 0 &&
        number[10] > 0 &&
        number[ 9] > 0) {
      straight = 13;
    } else {
      //other
      for (int i = 13 - 1; i >= 5; i--) {
        if (number[i + 0] > 0 &&
            number[i + 1] > 0 &&
            number[i + 2] > 0 &&
            number[i + 3] > 0 &&
            number[i + 4] > 0) {
          straight = i;
          break;
        }
      }
    }
  }

  // Check straight flush
  if (straight >= 0 && (straight == flushKey || straight == 13 && flushKey == 0)) {
    int count = 0;
    for (int i = 0; i < ncards; i++) {
      if (GET_COLOR(cards[i]) != flush) continue;
      int rank = GET_NUMBER(cards[i]);
      if (rank <= straight && rank >= straight - 5 || rank == 0 && straight ==13) {
        count++;
      }
    }
    if (count >= 5)
      straightFlush = true;
  }

  int ret;

  if (straightFlush) {
    ret = MAKE_HAND(STRAIGHT_FLUSH, 0, 0, 0);
  } else if (fourCard >= 0) {
    ret = MAKE_HAND(FOUR_CARD, CNV_ACE(fourCard), 0, 0);
  } else if (threeCard >= 0 && npair > 0) {
    ret = MAKE_HAND(FULL_HOUSE, CNV_ACE(threeCard), CNV_ACE(pair[0]), 0);
  } else if (flush >= 0) {
    ret = MAKE_HAND(FLUSH, CNV_ACE(flushKey), 0, 0);
  } else if (straight >= 0) {
    ret = MAKE_HAND(STRAIGHT, CNV_ACE(straight), 0, 0);
  } else {
    //  3,2,1,no pair need kicker
    int kicker;
    if (number[0] == 1) kicker = 13;
    for (int i = 13 - 1; i > 0; i--) {
      if (number[i] == 1) {
        kicker = i;
        break;
      }
    }

    if (threeCard >= 0) {
      ret = MAKE_HAND(THREE_CARD, CNV_ACE(threeCard), 0, kicker);
    } else if (npair == 2) {
      ret = MAKE_HAND(TWO_PAIR, CNV_ACE(pair[0]), CNV_ACE(pair[1]), kicker);
    } else if (npair == 1) {
      ret = MAKE_HAND(PAIR, CNV_ACE(pair[0]), 0, kicker);
    } else {
      ret = MAKE_HAND(NONE, 0, 0, kicker);
    }
  }

  return ret;
}
