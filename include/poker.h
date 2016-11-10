#ifndef __POKER_H
#define __POKER_H

#include <assert.h>

#include "mtseq.h"

typedef unsigned int Card;

static inline Card MAKE_CARD(int suit, int number) {
  return (suit << 4) | (number);
}

static inline int GET_COLOR(Card c) {
  return (c >> 4) & 0xf;
}

static inline int GET_NUMBER(Card c) {
  return (c) & 0xf;
}

static inline int GET_HOLE(int big, int small, bool same) {
  int p = same ?
    (big - 1) * 13 + (small - 1) :
    (small - 1) * 13 + (big - 1);
  assert(p >= 0 && p < 13 * 13);
  return p;
}

//
enum {
  HICARD = 0,
  PAIR,
  TWO_PAIR,
  THREE_CARD,
  STRAIGHT,
  FLUSH,
  FULL_HOUSE,
  FOUR_CARD,
  STRAIGHT_FLUSH,
  HAND_MAX
};


// Make hand excluded high card and one pair
static inline int MAKE_HAND(int TYPE, int RANK1 = 0, int RANK2 = 0) {
  return (TYPE << 16) | (RANK1 << 12) | (RANK2 << 8);
}

static inline int MAKE_HICARD() {
  return (HICARD << 16);
}

static inline int GET_HAND(int x) {
  return (x >> 16) & 0xf;
}

static inline int GET_RANK1(int x) {
  int a;
  if (GET_HAND(x) == HICARD) {
    a = 0;
    for (int i = 13 - 1; i >= 1; i--) {
      if ((x >> (i - 1)) & 1) {
        a = i;
        break;
      }
    }
  } else {
    a = (x >> 12) & 0xf;
  }
  return a == 12 ? 1 : a + 2;
}

static inline int GET_RANK2(int x) {
  int a = (x >> 8) & 0xf;
  return a == 12 ? 1 : a + 2;
}

static inline int GET_RANK3(int x) {
  int a = (x >> 4) & 0xf;
  return a == 12 ? 1 : a + 2;
}

static inline int GET_RANK4(int x) {
  int a = (x >> 0) & 0xf;
  return a == 12 ? 1 : a + 2;
}

static inline int GET_HICARD_KICKER(int x) {
  return x & 0xfff;
}

static inline int CNV_ACE(int x) {
  return x == 1 ? 14 : x;
}

#define CARD_MAX 16

static inline int CNV_SIMPLIFIED_HAND(int hand) {
  return GET_HAND(hand) * CARD_MAX + (GET_RANK1(hand) - 1);
}

static inline int GET_SIMPLIFIED_HAND(int hand) {
  return hand / CARD_MAX;
}

static inline int GET_SIMPLIFIED_RANK1(int hand) {
  return CNV_ACE((hand % CARD_MAX) + 1);
}

#define CLEAR(array) for (int i = 0; i < sizeof(array) / sizeof(*array); i++) array[i] = 0;
#define CLEAR_LEN(array, len) for (int i = 0; i < len; i++) array[i] = 0;

class Hand {
public:
  static const char *handName[16];
  static const char rankName[];
  static const char suitName[];
  static int rankName2Number(char);
  static int suitName2Number(char);

  /**
     @return number of read cards
   */
  static int str2cards(const char* in, Card* card, int len);

  static int calcHand(const char* cards) {
    Hand hand(cards);
    return hand.calc();
  }

  Hand() {
    clear();
  }

  Hand(const char *cards) {
    clear();
    put(cards);
  }

  void clear() {
    CLEAR(color);
    CLEAR(number);
    CLEAR(cards);
    ncards = 0;
  }

  void put(Card card) {
    int c = GET_COLOR(card);
    int n = GET_NUMBER(card);
    color[c]++;
    if (n == 1) n = 12; else n -= 2;
    number[n]++;
    cards[ncards++] = card;
  }

  bool put(const char *in);

  int calc();

  static const int holeSize = 13 * 13;
  int getHole() const {
    return getHole(cards[0], cards[1]);
  }

  static int getHole(Card a, Card b) {
    bool same = GET_COLOR(a) == GET_COLOR(b);
    int x = GET_NUMBER(a);
    int y = GET_NUMBER(b);
    if (CNV_ACE(x) < CNV_ACE(y)) {
      int t = y;
      y = x;
      x = t;
    }

    return GET_HOLE(x, y, same);
  }

  static void printCard(Card c) {
    putchar(rankName[GET_NUMBER(c)]);
    putchar(suitName[GET_COLOR(c)]);
  }

  void print(int s = 0) {
    for (int i = s; i < ncards; i++) {
      printCard(cards[i]);
    }
  }

  static const int simplifiedHandSize = (HAND_MAX + 1) * CARD_MAX;

private:
  int color[4];
  int number[13];
  int cards[7];
  int ncards;
};

class Deck {
public:
  Deck() {
    init();
  }

  void init() {
    int k = 0;
    for (int i = 0; i < 4; i++) {
      for (int j = 1; j <= 13; j++) {
        cards[k++] = MAKE_CARD(i, j);
      }
    }
    cardNum = 52;
    cardPos = 0;
  }

  void shuffle() {
    for (int i = cardNum - 1; i >= 0; i--) {
      int p = mt.genrand_int31() % cardNum;
      int a = cards[i];
      cards[i] = cards[p];
      cards[p] = a;
    }
  }

  bool remove(Card a) {
    Card o = -1;
    for (int i = 0; i < cardNum; i++) {
      if (cards[i] == a) {
        for (; i < cardNum - 1; i++) cards[i] = cards[i + 1];
        cardNum--;
        if (cardPos >= cardNum) cardPos = 0;
        return true;
      }
    }
    return false;
  }

  Card pull() {
    int r = cards[cardPos];
    cardPos++;
    if (cardPos >= cardNum) cardPos = 0;
    return r;
  }

  Card cards[13 * 4];
  int cardPos;
  int cardNum;
  __NS_MTSEQ::MTSequence mt;
};

class PlayerResult {
public:
  PlayerResult() {
    init();
  }

  void init() {
    destroy();

    CLEAR(winnerHole);
    CLEAR(allHole);
    CLEAR(costByHole);
    CLEAR(incomeByHole);

    CLEAR(winnerHand);
    CLEAR(allHand);
  }

  void destroy() {
  }

  ~PlayerResult() {
    destroy();
  }

  void dump(FILE *fp) const;
  bool restore(FILE *fp);
  void show(int nplayer, int nrepeat) const;
  void showWinningHands(int nrepeat, int nplayer) const;
  void showWinningRatioEachHands(int nplayer) const;
  void showRatioOfWinningHands() const;
  void showWinningRatioByHole(int nplayer) const;
  void showEVByHole() const;
  void showEVPercentByHole() const;

  int getIncome() const;
  int getCost() const;

  PlayerResult& operator+=(const PlayerResult& a);

public:
  int strategy;

  int winnerHole[Hand::holeSize];
  int allHole[Hand::holeSize];

  int costByHole[Hand::holeSize];
  int incomeByHole[Hand::holeSize];

  int winnerHand[Hand::simplifiedHandSize];
  int allHand[Hand::simplifiedHandSize];
};

class Result {
public:
  Result() {
    ncaller = 0;
    players = 0;
  }

  Result(int nplayer, int nstrategy) {
    ncaller = 0;
    players = 0;
    init(nplayer, nstrategy);
  }

  void init(int nplayer, int nstrategy) {
    destroy();

    this->nplayer = nplayer;
    this->nstrategy = nstrategy;

    ncaller = new int[nplayer + 1];
    CLEAR_LEN(ncaller, nplayer + 1);

    players = new PlayerResult[nplayer];
  }

  void destroy() {
    delete []ncaller;
    ncaller = 0;
    delete []players;
    players = 0;
  }

  bool dump(const char *fname) const;
  bool restore(const char *fname);

  void show() const;
  void showInThePotPlayers() const;

  int getStrategyResult(int strategy, PlayerResult *result) const;

public:
  int nplayer;
  int nrepeat;
  int nstrategy;

  int *ncaller;
  PlayerResult *players;
};

#endif //__POKER_H
