#ifndef __POKER_H
#define __POKER_H

typedef unsigned int Card;

static inline Card MAKE_CARD(int color, int number) {
  return (color << 4) | (number - 1);
}

static inline int GET_COLOR(Card c) {
  return (c >> 4) & 0xf;
}

static inline int GET_NUMBER(Card c) {
  return (c) & 0xf;
}

static inline int MAKE_HAND(int TYPE, int RANK1, int RANK2, int KICKER) {
  return (TYPE << 12) | (RANK1 << 8) | (RANK2 << 4) | KICKER;
}

static inline int GET_HAND(int x) {
  return (x >> 12) & 0xf;
}

static inline int GET_RANK1(int x) {
  return (x >> 8) & 0xf;
}

static inline int GET_RANK2(int x) {
  return (x >> 4) & 0xf;
}

static inline int GET_KICKER(int x) {
  return (x >> 0) & 0xf;
}

static inline int CNV_ACE(int k) {
  return k == 0 ? 13 : k;
}

#define CLEAR(array) for (int i = 0; i < sizeof(array) / sizeof(*array); i++) array[i] = 0;

class Hand {
public:
  enum {
    NONE = 0,
    PAIR,
    TWO_PAIR,
    THREE_CARD,
    STRAIGHT,
    FLUSH,
    FULL_HOUSE,
    FOUR_CARD,
    STRAIGHT_FLUSH
  };

  Hand() {
    CLEAR(color);
    CLEAR(number);
    CLEAR(cards);
    ncards = 0;
  }

  void put(Card card) {
    int c = GET_COLOR(card);
    int n = GET_NUMBER(card);
    color[c]++;
    number[n]++;
    cards[ncards++] = card;
  }

  int calc();

private:
  int color[4];
  int number[13];
  int cards[7];
  int ncards;
};

#endif //__POKER_H
