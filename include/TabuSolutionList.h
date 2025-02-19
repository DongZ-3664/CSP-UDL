#ifndef _TabuSolutionList_H_
#define _TabuSolutionList_H_

#include "TabuObject.h"

class TabuSolutionList
{
public:
	TabuSolutionList(int cap, int tenure, double forget_rate);
	TabuSolutionList();
	~TabuSolutionList();

	bool Find(Solution* sol);
	void Remember(Solution* sol);

	void DeleteEarliest();
	void DeletePart(int s_pos, int e_pos);
	void ClearMemory();


	int GetSize();
	int GetTenure();

	void PrintTabuList();

	void Updata();

private:
	int capatity;
	int tabu_tenure;
	double forget_length;

	std::vector<TabuObject*> tabu_list;

	std::map<double, bool> exist_flag;
	
	
};









#endif // !_TabuSolutionList_H_
