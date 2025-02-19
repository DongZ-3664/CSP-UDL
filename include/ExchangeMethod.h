#ifndef _ExchangeMethod_H_
#define _ExchangeMethod_H_

#include "TabuOperatorList.h"

struct ExPair
{
	ExPair(int x1_id, int x2_id, double rc)
	{
		job1 = x1_id;
		job2 = x2_id;
		reduce_cost = rc;
	}

	~ExPair()
	{

	}

	int job1;
	int job2;

	double reduce_cost;
};


class ExchangeMethod
{
public:
	ExchangeMethod(std::string	name);
	~ExchangeMethod();


	void GenerateEx3Que(Solution* Sol);

	void InitialFlag(int n);
	bool CanbeExchange(int p_pos, int d_pos);
	void SortQueue();
	void ForbidEx(int p_pos, int d_pos);
	void PrintExchangeQue(int num_ex_jobs);
	
	std::vector<int> GetMoveJobs();





	//===============test modulr==================
	void TestExChange2Jobs(Solution*& Sol, TabuOperatorList* tbls);


private:

	void TestGenerateEx2Que(Solution* Sol, TabuOperatorList* tbls);



	std::string OperatorName;

	std::vector<ExPair*> ExchangeQue;	
	
	std::vector<bool> _Ex_Flag;

	std::vector<int> mv_job_vec;

	int* _pos_of_node = NULL;	//The position of node[i] in the current solution [0x, (1,2,.......,2N)]
};




#endif // !_ExchangeMethod_H_