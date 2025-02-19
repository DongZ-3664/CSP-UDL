#include "Compare.h"

bool cmp1(const Record& IP1, const Record& IP2)
{
	return IP1.InCost > IP2.InCost;
}

bool cmp2(const Record& IP1, const Record& IP2)
{
	return IP1.InCost < IP2.InCost;
}

bool cmp4(const std::pair<int, double>& P1, const std::pair<int, double>& P2)
{
	return P1.second > P2.second;
}

bool cmp5(const std::pair<int, double>& P1, const std::pair<int, double>& P2)
{
	return P1.second < P2.second;
}

bool cmp7(ExPair* p1, ExPair* p2)
{
	return p1->reduce_cost < p2->reduce_cost;
}

bool cmp8(Solution* sol1, Solution* sol2)
{
	return sol1->GetTotalCosts() < sol2->GetTotalCosts();
}
