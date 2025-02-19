#ifndef _RelocateMethod_H_
#define _RelocateMethod_H_

#include "CandidateCouple.h"
#include "TabuOperatorList.h"


class RelocateMethod
{
public:
	RelocateMethod(std::string name);
	~RelocateMethod();
	
	void PrintCandidateQueue();

	int GetCandidateQueueSize();

	void ClearQueue();

	std::vector<int> GetMoveJobs();

	void GenerateCandidateQue(Solution* sol, TabuOperatorList* tbls);

	void RelocateMultiCouple(Solution*& sol);

	std::vector<Record> GainRecordQueOFJob(Solution* sol, int job_id, double cost_rm);

	void Optimize(Solution*& sol, TabuOperatorList* tbls);

	bool CanBeRemoved(Solution* sol, int p_pos, int d_pos);
	bool CanBeInserted(Solution* sol, int p_pos, int d_pos);

	void ForbidRmove_Range(Solution* sol, int start_pos, int end_pos);
	void ForbidRmove_Unit(Solution* sol, int pos);
	void ForbidInsert_Range(Solution* sol, int start_pos, int end_pos);
	void ForbidInsert_Unit(Solution* sol, int pos);

private:

	std::string OperatorName;

	// the candidate set
	std::priority_queue<CandidateCouple, std::vector<CandidateCouple>, cmp3> Candidatequeue;

	std::vector<int> mv_job_vec;

	//===============test module==========================
	int* _pos_of_node = NULL;
	bool* _In_Flag_of_node = NULL;
	bool* _Rm_Flag_of_node = NULL;
};

#endif // !_RelocateMethod_H_
