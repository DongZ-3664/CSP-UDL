#ifndef _TabuObject_H_
#define _TabuObject_H_

#include "Solution.h"

struct TabuObject
{
	TabuObject(std::vector<int> sp, double tc);
	~TabuObject();

	bool operator==(TabuObject& ob);
	
	std::ostream& operator<<(std::ostream& os);

	std::vector<int> sol_path;
	double total_cost;

	int rest = 30;
};



#endif //_TabuObject_H_