#ifndef __STRATEGY_H
#define __STRATEGY_H

#include "poker.h"
#include "table.h"

class Strategy {
public:
  static Strategy* load(const char* fname);

  virtual Action calc(Hand *hand, TableInfo* dat) = 0;
  virtual bool processConfigLine(const char* line) {return true;}
  virtual bool processConfigFinish() {return true;}
};


#endif //__STRATEGY_H
