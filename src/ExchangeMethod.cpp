#include "ExchangeMethod.h"

extern bool cmp7(ExPair* p1, ExPair* p2);

ExchangeMethod::ExchangeMethod(std::string name)
{
	this->OperatorName = name;
}


ExchangeMethod::~ExchangeMethod()
{

}

void ExchangeMethod::GenerateEx3Que(Solution* Sol)
{
	this->ExchangeQue.clear();
	int N = Sol->GetNbJobs();

	double sol_cost = Sol->GetTotalCosts();
	for (int x_1 = 1; x_1 <= N - 2; ++x_1)
	{
		for (int x_2 = x_1 + 1; x_2 <= N - 1; ++x_2)
		{
			for (int x_3 = x_2 + 1; x_3 <= N; ++x_3)
			{

				Solution* cur_sol = new Solution(Sol);

				int x_1_p = cur_sol->IndexOf(x_1);
				int x_1_d = cur_sol->IndexOf(x_1 + N);
				int x_2_p = cur_sol->IndexOf(x_2);
				int x_2_d = cur_sol->IndexOf(x_2 + N);
				int x_3_p = cur_sol->IndexOf(x_3);
				int x_3_d = cur_sol->IndexOf(x_3 + N);


				cur_sol->ChangeNode(x_3, x_1_p);
				cur_sol->ChangeNode(x_3 + N, x_1_d);
				cur_sol->ChangeNode(x_1, x_2_p);
				cur_sol->ChangeNode(x_1 + N, x_2_d);
				cur_sol->ChangeNode(x_2, x_3_p);
				cur_sol->ChangeNode(x_2 + N, x_3_d);

				double cur_cost = cur_sol->CalculateTotalCosts();

				if (cur_cost < sol_cost)
				{
					double delta = sol_cost - cur_cost;
					int cur_ex_job[] = { x_1, x_2, x_3 };
					int cur_ex_pos[] = { x_1_p,x_1_d, x_2_p,x_2_d, x_3_p,x_3_d };
				}
			}
		}
	}

	return;
}

void ExchangeMethod::InitialFlag(int n)
{
	this->_Ex_Flag.clear();
	this->_Ex_Flag.resize(n, 1);
}

void ExchangeMethod::ForbidEx(int p_pos, int d_pos)
{
	if (d_pos < p_pos)
		throw IllegalParameters("Input parameters error !");
	if (p_pos == -1) ++p_pos;
	if (d_pos == this->_Ex_Flag.size()) --d_pos;
	for (int i = p_pos; i <= d_pos; ++i)
	{
		this->_Ex_Flag[i] = 0;
	}
	return;
}

bool ExchangeMethod::CanbeExchange(int p_pos, int d_pos)
{
	for (int i = p_pos; i <= d_pos; ++i)
	{
		if (this->_Ex_Flag[i] == 0)
			return false;
	}
	return true;
}

void ExchangeMethod::SortQueue()
{
	if (this->ExchangeQue.empty()) { return; }
	std::sort(this->ExchangeQue.begin(), this->ExchangeQue.end(), cmp7);
}




std::vector<int> ExchangeMethod::GetMoveJobs()
{
	return this->mv_job_vec;
}



//=====================test module==============================
void ExchangeMethod::TestGenerateEx2Que(Solution* Sol, TabuOperatorList* tbls)
{
	this->ExchangeQue.clear();
	const int N = Sol->GetNbJobs();

	double sol_cost = Sol->GetTotalCosts();

	for (int x_1 = 1; x_1 < N; ++x_1)
	{
		if (tbls->Find(x_1))
		{
			continue;
		}

		
		double best_cost = sol_cost;
		int bst_x2 = -1;

		for (int x_2 = x_1 + 1; x_2 <= N; ++x_2)
		{
			if (tbls->Find(x_2))
			{
				continue;
			}


			Sol->ChangeJob(x_2, _pos_of_node[x_1], _pos_of_node[x_1 + N]);
			Sol->ChangeJob(x_1, _pos_of_node[x_2], _pos_of_node[x_2 + N]);
			double cur_cost = Sol->GetTotalCosts();


			//¸´Ô­²Ù×÷	
			Sol->ChangeJob(x_1, _pos_of_node[x_1], _pos_of_node[x_1 + N]);
			Sol->ChangeJob(x_2, _pos_of_node[x_2], _pos_of_node[x_2 + N]);


			if (cur_cost < best_cost)
			{
				best_cost = cur_cost;

				bst_x2 = x_2;
			}

		}

		if (sol_cost - best_cost > 0.1)
		{
			ExPair* cur_record = new ExPair(x_1, bst_x2, sol_cost - best_cost);
			this->ExchangeQue.push_back(cur_record);
		}
	
	}


	return;
}
	



void ExchangeMethod::TestExChange2Jobs(Solution*& Sol, TabuOperatorList* tbls)
{
	int N = Sol->GetNbJobs();

	_pos_of_node = new int[2 * N + 1];
	_pos_of_node[0] = -1;
	for (auto i = 0u; i < 2 * N; ++i)
	{
		_pos_of_node[Sol->NodeAt(i)] = i;
	}

	this->TestGenerateEx2Que(Sol, tbls);

	int cur_runs = 0;
	while (!this->ExchangeQue.empty())
	{
		this->InitialFlag(2 * N);
		this->SortQueue();

		//std::cout << "current quene size : " << this->ExchangeQue.size() << std::endl;


		while (!this->ExchangeQue.empty())
		{
			ExPair* cur_ex = this->ExchangeQue.back();
			this->ExchangeQue.pop_back();

			int x_1 = cur_ex->job1;
			int x_2 = cur_ex->job2;
			double delta = -(cur_ex->reduce_cost);
			
			bool a = CanbeExchange(_pos_of_node[x_1], _pos_of_node[x_1 + N]);
			bool b = CanbeExchange(_pos_of_node[x_2], _pos_of_node[x_2 + N]);


			if (a && b)
			{
				Sol->ChangeJob(x_1, _pos_of_node[x_2], _pos_of_node[x_2 + N]);
				Sol->ChangeJob(x_2, _pos_of_node[x_1], _pos_of_node[x_1 + N]);
			


				this->ForbidEx(_pos_of_node[x_1] - 1, _pos_of_node[x_1 + N] + 1);
				this->ForbidEx(_pos_of_node[x_2] - 1, _pos_of_node[x_2 + N] + 1);


				//Î¬»¤_pos_of_node
				std::swap(_pos_of_node[x_1], _pos_of_node[x_2]);
				std::swap(_pos_of_node[x_1 + N], _pos_of_node[x_2 + N]);


				this->mv_job_vec.push_back(x_2);
				this->mv_job_vec.push_back(x_1);

			}
			cur_ex->~ExPair();
		}
				
		this->TestGenerateEx2Que(Sol, tbls);
	}



	delete[] _pos_of_node;
	_pos_of_node = NULL;


	this->mv_job_vec.clear();
	return;

}
