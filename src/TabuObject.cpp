#include "TabuObject.h"

TabuObject::TabuObject(std::vector<int> sp, double tc)
{
	this->sol_path = sp;
	this->total_cost = tc;
}

TabuObject::~TabuObject()
{
	
}

bool TabuObject::operator==(TabuObject& ob)
{
	if (this->total_cost != ob.total_cost)
	{
		return false;
	}
	for (auto i = 0; i < sol_path.size(); ++i)
	{
		if (sol_path[i] != ob.sol_path[i])
			return false;
	}
	return true;
}


std::ostream& TabuObject::operator<<(std::ostream& os)
{
	std::cout << this->total_cost;
	return os;
}




