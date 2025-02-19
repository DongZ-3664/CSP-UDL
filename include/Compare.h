#ifndef _Compare_H_
#define _Compare_H_

#include "RelocateMethod.h"
#include "ExchangeMethod.h"


bool cmp1(const Record& IP1, const Record& IP2);
bool cmp2(const Record& IP1, const Record& IP2);
bool cmp4(const std::pair<int, double>& P1, const std::pair<int, double>& P2);

bool cmp5(const std::pair<int, double>& P1, const std::pair<int, double>& P2);

bool cmp7(ExPair* p1, ExPair* p2);

bool cmp8(Solution* sol1, Solution* sol2);


#endif // !_Compare_H_
