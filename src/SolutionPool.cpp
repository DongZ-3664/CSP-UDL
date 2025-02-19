#include "SolutionPool.h"


extern bool cmp8(Solution* sol1, Solution* sol2);


SolutionPool::SolutionPool(int m_min, double m_ratio)
{
	pool_min_size = m_min;
	ratio = m_ratio;
}


SolutionPool::SolutionPool()
{

}


SolutionPool::~SolutionPool()
{

}

bool SolutionPool::Find(Solution* sol)
{
	for (auto i = 0u; i < pool.size(); i++)
	{
		if ((*pool[i]) == (*sol)) { return true; }
	}
	return false;

}

void SolutionPool::Remember(Solution* sol)
{
	if (Find(sol)) { return; }

	Solution* cpsol = new Solution(sol);
	this->pool.push_back(cpsol);
	return;

}

void SolutionPool::ClearMemory()
{
	pool.clear();
	return;
}

void SolutionPool::CutUseless()
{
	if (pool.empty()) { return; }
	int r = std::ceil(pool.size() * ratio);
	int erase_start = std::min(15, r);
	erase_start = std::max(erase_start, 1);

	std::sort(pool.begin(), pool.end(), cmp8);
	pool.erase(pool.begin() + erase_start, pool.end()); //solution_pool [0 : erase_start - 1];

	std::cerr << "solution pool size : " << pool.size() << std::endl;

	
	if (pool.size() < pool_min_size)
	{
		throw IllegalParameters("without enough high-quality solution !");
	}
	
}

void SolutionPool::GetFixedVariables(int**& x_count, int**& y_count, int**&w_count, int N)
{
	x_count = new int* [2 * N + 1];
	y_count = new int* [2 * N + 1];
	


	for (auto i = 0u; i < 2 * N + 1; ++i)
	{
		x_count[i] = new int[2 * N + 2];
		y_count[i] = new int[2 * N + 2];	

		for (auto j = 0u; j < 2 * N + 2; ++j)
		{
			x_count[i][j] = 0;
			y_count[i][j] = 0;
		}
	}

	//  w count
	w_count = new int* [N + 1];
	for (auto i = 1u; i <= N; ++i) {
		w_count[i] = new int[N + 1];
		for (auto j = 1u; j <= N; ++j) {
			w_count[i][j] = 0;
		}
	}



	for (Solution* sol : pool)
	{
		++x_count[0][sol->NodeAt(0)];

		for (int cur_index = 0; cur_index < 2 * N - 1; ++cur_index)
		{
			int x1 = sol->NodeAt(cur_index);
			int x2 = sol->NodeAt(cur_index + 1);
			
			bool a = x1 <= N;
			bool b = x2 <= N;

			if (a ^ b)
			{
				if (sol->LoadAtPos(cur_index) == 2 || sol->LoadAtPos(cur_index + 1) == 2)
				{
					++y_count[x1][x2];
					continue;
				}
			}

			++x_count[x1][x2];

		}
		
		++x_count[sol->NodeAt(2 * N - 1)][2 * N + 1];


		for (int cur_index = 0; cur_index < 2 * N - 1; ++cur_index) {
			if (sol->NodeAt(cur_index) <= N && sol->NodeAt(cur_index + 1) <= N) {
				int cur_job = sol->NodeAt(cur_index);
				for (int j = cur_index + 1; sol->NodeAt(j) != cur_job + N; j += 2) {
					++w_count[cur_job][sol->NodeAt(j)];	
				}
			}	
		}
	}


}

void SolutionPool::PrintPool(std::ostringstream& osos, double lower_bound)
{
	osos << "Solution Pool Information : " << std::endl;
	int i = 0;
	for (Solution* sol : pool)
	{
		osos << "solution " << ++i << " : " << sol->GetTotalCosts();
		if (lower_bound != 0.0)
		{
			osos << ", Gap : " << 100 * (sol->GetTotalCosts() - lower_bound) / sol->GetTotalCosts() << "%.";
		}
		osos << std::endl;


	}

}

int SolutionPool::GetPoolSize()
{
	return pool.size();
}




//=========================================10.08 evolutionary algorithms==============================
#if _RUN_POPULATION
void SolutionPool::Evolve()
{
	//size of initial population = 30; iterations = 100;

	Instance* test_instance = pool[0]->GetInstance();
	const int num_N = pool[0]->GetNbJobs();

	//main loop
	int cur_it = 0;
	const int max_it = 100;
	std::vector<Solution*> population;
	while (++cur_it <= max_it)
	{


		// traversal, as one iteration;
		for (int p1 = 0; p1 < pool.size(); ++p1) {



			int p2 = RandNum(0, GetPoolSize() - 1);

			std::vector<int> parent_1 = pool[p1]->GetPath();
			std::vector<int> parent_2 = pool[p2]->GetPath();

			Solution* ps1 = pool[p1];
			Solution* ps2 = pool[p2];


			// create offspring 1
			//choose break point [x, y] = [cross_start, cross_end]
			int cross_start = RandNum(0, num_N);
			while (parent_1[cross_start] >= num_N + 1 || ps1->LoadAtPos(cross_start) != 1) { ++cross_start; }
			if (cross_start >= 2 * num_N) { cross_start = 0; }
			int cross_end = RandNum(cross_start + 1, 2 * num_N - 1);
			while (parent_1[cross_end] <= num_N || ps1->LoadAtPos(cross_end) != 0) { ++cross_end; }

			std::vector<int> offspring_1(cross_end - cross_start + 1);
			{			
				std::copy(parent_1.begin() + cross_start, parent_1.begin() + cross_end + 1, offspring_1.begin());
				std::map<int, bool> crossed;
				for (const int& a : offspring_1) {  crossed[a] = true; }

				int cur_index = (cross_end + 1) % (2 * num_N);	//确定填充起点
				while (parent_2[cur_index] >= num_N + 1 || ps2->LoadAtPos(cur_index) != 1) { cur_index = (cur_index + 1) % (2 * num_N);}

				while (offspring_1.size() < 2 * num_N)
				{
					int the_oper = parent_2[cur_index];
					if (crossed[the_oper] != true) {
						offspring_1.emplace_back(the_oper);		
					}
					cur_index = (cur_index + 1) % (2 * num_N);
				}

				/*
					check procedure
				*/
			}


			// create offspring 2

			while (parent_2[cross_start] >= num_N + 1 || ps2->LoadAtPos(cross_start) != 1) { --cross_start; }
			while (parent_2[cross_end] <= num_N || ps2->LoadAtPos(cross_end) != 0) { ++cross_end; }
			std::vector<int> offspring_2(cross_end - cross_start + 1);
			{
				std::copy(parent_2.begin() + cross_start, parent_2.begin() + cross_end + 1, offspring_2.begin());
				std::map<int, bool> crossed;
				for (const int& a : offspring_2) { crossed[a] = true; }
				

				int cur_index = (cross_end + 1) % (2 * num_N);	//确定填充起点
				while (parent_1[cur_index] >= num_N + 1 || ps1->LoadAtPos(cur_index) != 1) { cur_index = (cur_index + 1) % (2 * num_N); }

				while (offspring_2.size() < 2 * num_N)
				{
					int the_oper = parent_1[cur_index];
					if (crossed[the_oper] != true) {
						offspring_2.emplace_back(the_oper);
					}
					cur_index = (cur_index + 1) % (2 * num_N);
				}
				/*
					check procedure
				*/
			}


			Solution* offspring_sol_1 = new Solution(offspring_1, test_instance);
			Solution* offspring_sol_2 = new Solution(offspring_2, test_instance);

			if (!offspring_sol_1->IsFeasible() || !offspring_sol_2->IsFeasible())
			{
				offspring_sol_1->PrintSolutionInformation();
				offspring_sol_2->PrintSolutionInformation();
				throw IllegalParameters("infeasible!!!!!!!!!!!!!");
			}


			if (offspring_sol_1->GetTotalCosts() < ps1->GetTotalCosts()) { delete pool[p1]; pool[p1] = new Solution(offspring_sol_1); ++num_crossover_Imp; }
			if (offspring_sol_2->GetTotalCosts() < ps2->GetTotalCosts()) { delete pool[p2]; pool[p2] = new Solution(offspring_sol_2); ++num_crossover_Imp; }
			delete offspring_sol_1;
			delete offspring_sol_2;
		}

	}

	std::sort(pool.begin(), pool.end(), cmp8);
}
#endif // _RUN_POPULATION

std::vector<Solution*> SolutionPool::GetPool()
{
	return this->pool;
}


void SolutionPool::InitializePopulation()
{
	int erase_start = 40;

	if (pool.size() < 40)
	{
		throw IllegalParameters("number of elite solutions less than 40");
	}

	std::sort(pool.begin(), pool.end(), cmp8);
	pool.erase(pool.begin() + erase_start, pool.end()); //solution_pool [0 : erase_start - 1];

	std::cerr << "solution pool size : " << pool.size() << std::endl;

}






