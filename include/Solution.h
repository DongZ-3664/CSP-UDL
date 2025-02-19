#ifndef _Solution_H_
#define _Solution_H_

#include "Instance.h"


class Solution		 
{										
public:
	Solution(std::vector<int>& sol_vec, Instance* ins);
	Solution(Solution* sol);

	Solution();
	~Solution();




	void PrintSolutionInformation();

	void RemoveLoad(int start_pos, int end_pos);

	void UpDataLoadStatus(int start_pos, int end_pos);


	int CurrentMaxLoad();

	int LoadAtPos(int pos);
	
	int GetDeliveryNode(int pickup_node_id);

	double CalculateTotalCosts();
	double GetTotalCosts();
	double IntervalCosts(int start_pos, int end_pos);

	bool operator==(Solution& sol);

	bool IsFeasible();

	std::vector<int> GetDInsertedSuc(int pos);
	std::vector<int> GetDInsertedPos(int pos);

	std::vector<int> ConvertToIndex(std::vector<int>& a_suc_vec);

	double	GainInertionAdd(int job_id, int p_in_pos, int d_in_pos);
	double	GainRemovalSub(int job_id, int rx_p_pos = -1, int rx_d_pos = -1);

	Solution* RemoveNodeAtPosition(int pos);
	Solution* InsertNodeAtPosition(int node, int pos);
	Solution* RemoveJob(int job_id, int rm_p_pos = -1, int rm_d_pos = -1);

	Instance* GetInstance();

	void ChangeNode(int job_id, int pos);
	void ChangeJob(int job_id, int p_pos, int d_pos);
	bool IsPickUpNode(int pos);
	int IndexOf(int node);
	int NodeAt(int pos);
	bool CanBeReversed();				
	const int GetNbJobs();
	int GetNbNodes();
	std::vector<int> GetPath();
	std::vector<int> GetRelativeJob(int rx_p_pos, int rx_d_pos);

	std::vector<int> GetRelativeJobTEST(int rx_p_pos, int rx_d_pos);


	void ChangeCost(double delta);

	int Operation(int pos);

	int getsize() { return XPath.size(); }
private:

	Instance* CurrentInstance;

	double TotalCosts; 

	int NbNodes;
	int NbJobs;

	//The path of current solution 
	std::vector<int> XPath;

	//Load status
	std::vector<int> Load;


	
};

#endif // !_Solution_H_