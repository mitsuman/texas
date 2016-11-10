#ifndef __TABLE_H
#define __TABLE_H

class Hand;
class Strategy;

typedef int Action;
const int Fold = -1;
const int Unknown = 0;
const int Call = 1; // Call or Check
// raise if over 0.
// If new bet size is (old bet size * 2 + Action - Raise)
const int Raise = 2;

class TableInfo {
public:
  int nplayer;
  int nstrategy;
  Action *actions; // actions[0] is UTG
  int order; // 0 is UTG, nplayer-1 is BB
  int nraiser;
  int lastRaiser; // 0 is UTG
  int nrestPlayer; // 0 is UTG

  int pot;
  int bet;
  int BB;
};

class Table : public TableInfo{
public:
  Table();
  ~Table();

  void destroy();
  bool loadConfig(const char *fname);
  void calc();
  bool dump(const char *fname);

  void initPlayer(int number) {
    nplayer = number;
    players = new int[nplayer];
  }

  void initStrategy(int number) {
    nstrategy = number;
    strategies = new Strategy*[nstrategy];
  }

  bool loadStrategy(int n, const char *fname);
  bool setStrategy(int player, int strategy);

public:
  Result results;

protected:
  virtual void serveHole();
  virtual void serveFlop();
  virtual void serveTurn();
  virtual void serveRiver();
  void betting();
  void detectWinner();
  virtual void record(int nwinner, int highHand);

  void serveCommunityCard(int n);
  void prepare();

protected:
  Deck deck;
  int utg;

  Strategy **strategies;
  int *players; // strategy

  // variables for single game
  Hand *hands; // hands[0] is players[0]'s
  int *winners;

  bool debug;
};

#endif //__TABLE_H
