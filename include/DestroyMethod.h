#ifndef _DestroyMethod_H_
#define _DestroyMethod_H_


#include "Compare.h"
#include "TabuOperatorList.h"

class DestroyMethod
{
public:
	friend class RepairMethod;

	DestroyMethod(std::string name, int num);
	~DestroyMethod();
	
	int RandomAPos(int sol_length, int random_degree);
	void WorstRemoval(Solution*& ISol, TabuOperatorList* tbls, int rand_degree);

	

	void NearestRemoval(Solution*& ISol, TabuOperatorList* tbls, int rand_degree);
	void RandomRemoval(Solution*& ISol, TabuOperatorList* tbls, int rand_degree);
	void PrintRmJobs();

	void SetScore(double s1 = _INITIAL_SCORE, double s2 = _INITIAL_SCORE, double s3 = _INITIAL_SCORE);
	double GetScore(int method_type);
	void UpdataWeights(int method_type, double delta);


	int ChooseARemovalMethod();

	std::vector<int> GetMoveJobs();

private:
	/*
	* This vector nemad rm_job_vec consist of a set of rm_job pairs, 
	* whose first element(int) imply that the id of rm_job, and second 
	* element(double) refer to the rm_cost of rm_job;
	*/
	int num_rm_jobs;
	std::string OperatorName;
	std::vector<int> rm_job_vec;

	double** RememberList;


	double random_removal_score = _INITIAL_SCORE;
	double nearest_removal_score = _INITIAL_SCORE;
	double worst_removal_score = _INITIAL_SCORE;

	/*
	+ ---------- + ---------------------- + ---------------------- + ---------------------- +
	|   name	 |    random_removal	  |     nearest_removal    |    worst_removal       |    
	+----------- + ---------------------- + ---------------------- + ---------------------- +
	|   score	 |  random_removal_score  |  nearest_removal_score |  worst_removal_score   |
	+----------- + ---------------------- + ---------------------- + ---------------------- +
	*/
};



#endif // !_DestroyMethod_H_