#ifndef _RepairMethod_H_
#define _RepairMethod_H_

#include "Solution.h"
#include "DestroyMethod.h"

class RepairMethod
{
public:
	RepairMethod(std::string name);
	~RepairMethod();

	double GainRegretValue(Solution* ISol, int in_job_id);

	void RandCheapestInsertion(Solution*& ISol, DestroyMethod* DesM);

	void RegretInsertion(Solution*& ISol, DestroyMethod* DesM);
	void CheapestInsertion(Solution*& ISol, DestroyMethod* DesM);

	void SetScore(double s1 = _INITIAL_SCORE, double s2 = _INITIAL_SCORE, double s3 = _INITIAL_SCORE);
	double GetScore(int method_type);

	void UpdataWeights(int method_type, double delta);

	int ChooseAInsertionMethod();

private:
	std::string OperatorName;
	std::vector<Record> record_vec;


	int k_step = 3;

	double rand_cheapest_insertion_score = _INITIAL_SCORE;
	double regret_insertion_score = _INITIAL_SCORE;
	double cheapest_insertion_score = _INITIAL_SCORE;



	/*
	+ ---------- + -------------------------------- + ------------------------ + -------------------------- +
	|   name	 |     rand_cheapest_insertion	    |     regret_insertion     |     cheapest_insertion     |
	+----------- + -------------------------------- + ------------------------ + -------------------------- +
	|   score	 |  rand_cheapest_insertion_score   |  regret_insertion_score  |  cheapest_insertion_score  |
	+----------- + -------------------------------- + ------------------------ + -------------------------- +
	*/
};



#endif // !_RepairMethod_H_

