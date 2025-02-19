#ifndef _TabuOperatorList_H_
#define _TabuOperatorList_H_

#include "Solution.h"

class TabuOperatorList
{
public:
	TabuOperatorList(int cap, int tenure, double forget_rate);
	TabuOperatorList();
	~TabuOperatorList();

	bool Find(int tb_job);
	void Remember(int tb_job);

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


	std::vector<std::vector<int>> tabu_list;


	std::map<int, bool> exist_flag;	

};









#endif // !_TabuOperatorList_H_
