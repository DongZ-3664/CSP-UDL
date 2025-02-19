#include "InitialSolutionCreator.h"


InitialSolutionCreator::InitialSolutionCreator()
{

}

InitialSolutionCreator::~InitialSolutionCreator()
{

}


//Generate a initial solution use greedy insertion method;
void InitialSolutionCreator::GreedyInsert(Solution*& SolPtr, Instance* ins, int runs)
{
	//Define N as the number of jobs(requests);
	int N = ins->InsNbJobs;
	double bigM = 9999999;
	int cur_runs = 0;

	double BestCost = bigM;

	while (runs--)
	{
		std::vector<int> job_set_vec = ins->JobSet;

		unsigned a = RandNum(1, 50);
		std::srand(a);
		std::random_shuffle(job_set_vec.begin(), job_set_vec.end());

		std::vector<int> ISol_vec = { job_set_vec.back(), job_set_vec.back() + N };
		job_set_vec.pop_back();
		Solution* cur_sol = new Solution(ISol_vec, ins);

		double cur_cost = cur_sol->GetTotalCosts();

		while (!job_set_vec.empty())
		{
			int cur_job = job_set_vec.back();
			job_set_vec.pop_back();

			double best_cur_delta = bigM;
			int best_in_p_pos;
			int best_in_d_pos;

			for (auto In_p_pos = 0u; In_p_pos <= cur_sol->GetNbNodes(); In_p_pos++)
			{
				std::vector<int> In_d_pos_vec = cur_sol->GetDInsertedPos(In_p_pos);

				for (auto In_d_pos : In_d_pos_vec)
				{
					double	cur_delta = cur_sol->GainInertionAdd(cur_job, In_p_pos, In_d_pos);
					if (cur_delta < best_cur_delta)
					{
						best_cur_delta = cur_delta;
						best_in_p_pos = In_p_pos;
						best_in_d_pos = In_d_pos;
					}
				}
			}

			cur_sol->InsertNodeAtPosition(cur_job + N, best_in_d_pos);
			cur_sol->InsertNodeAtPosition(cur_job, best_in_p_pos);
			cur_sol->UpDataLoadStatus(best_in_p_pos, best_in_d_pos + 1);
			cur_sol->ChangeCost(best_cur_delta);
			cur_cost += best_cur_delta;

		}



		if (cur_cost < BestCost)
		{
			if (SolPtr != NULL) { SolPtr->~Solution(); }
			
			SolPtr = cur_sol;
			BestCost = cur_cost;
			continue;
		}
		cur_sol->~Solution();

	}
	//std::cout << "Initial Solution Cost : " << BestCost << std::endl;

}



void InitialSolutionCreator::CreatePopulation(std::vector<Solution*>& SolPop, Instance* ins, int runs)
{
	int N = ins->InsNbJobs;
	double bigM = 9999999;
	int cur_runs = 0;
	SolPop.clear();

	double BestCost = bigM;

	while (runs--)
	{
		std::vector<int> job_set_vec = ins->JobSet;

		unsigned a = RandNum(1, 50);

		std::srand(a);
		std::random_shuffle(job_set_vec.begin(), job_set_vec.end());

		std::vector<int> ISol_vec = { job_set_vec.back(), job_set_vec.back() + N };
		job_set_vec.pop_back();
		Solution* cur_sol = new Solution(ISol_vec, ins);
		double cur_cost = cur_sol->GetTotalCosts();

		while (!job_set_vec.empty())
		{
			int cur_job = job_set_vec.back();
			job_set_vec.pop_back();

			double best_cur_delta = bigM;
			int best_in_p_pos;
			int best_in_d_pos;

			for (auto In_p_pos = 0u; In_p_pos <= cur_sol->GetNbNodes(); In_p_pos++)
			{
				std::vector<int> In_d_pos_vec = cur_sol->GetDInsertedPos(In_p_pos);

				for (auto In_d_pos : In_d_pos_vec)
				{
					double	cur_delta = cur_sol->GainInertionAdd(cur_job, In_p_pos, In_d_pos);
					if (cur_delta < best_cur_delta)
					{
						best_cur_delta = cur_delta;
						best_in_p_pos = In_p_pos;
						best_in_d_pos = In_d_pos;
					}
				}
			}

			cur_sol->InsertNodeAtPosition(cur_job + N, best_in_d_pos);
			cur_sol->InsertNodeAtPosition(cur_job, best_in_p_pos);
			cur_sol->UpDataLoadStatus(best_in_p_pos, best_in_d_pos + 1);
			cur_cost += best_cur_delta;
		}

		SolPop.push_back(cur_sol);

	}

	return;
}