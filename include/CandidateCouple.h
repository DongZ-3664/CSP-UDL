#ifndef _CandidateCouple_H_
#define _CandidateCouple_H_

#include "Solution.h"

class Record
{
public:
	Record(int p_suc, int d_suc, double inc = 0.0, double rv = 0.0)
	{
		p_d_successor[0] = p_suc;
		p_d_successor[1] = d_suc;

		InCost = inc;
		RegretValue = rv;
	}

	Record() {};

	~Record()
	{

	}

	int p_d_successor[2];

	double InCost;

	double RegretValue;

};


class CandidateCouple
{
public:

	CandidateCouple(int id, std::vector<Record> recrd, double rm_cost = 0.0)
	{
		request_id = id;
		record_vec = recrd;
		RmCost = rm_cost;
	}

	~CandidateCouple()
	{
		
	}

	void POP()
	{
		this->record_vec.pop_back();
	}

	int request_id;
	double RmCost;
	std::vector<Record> record_vec;
};



struct cmp3
{
	bool operator()(CandidateCouple& CP1, CandidateCouple& CP2)
	{
		return CP1.record_vec.back().RegretValue < CP2.record_vec.back().RegretValue;
	}
};




#endif // !_CandidateCouple_H_