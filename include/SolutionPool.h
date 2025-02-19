#ifndef _SolutionPool_H_
#define _SolutionPool_H_


#include "Solution.h"

class SolutionPool
{
public:

	SolutionPool(int m_min, double m_ratio);
	SolutionPool();
	~SolutionPool();

	bool Find(Solution* sol);
	void Remember(Solution* sol);
	void ClearMemory();

	void PrintPool(std::ostringstream& osos, double lower_bound = 0.0);
	void GetFixedVariables(int**& x_count, int**& y_count, int**& w_count, int N);
	int GetPoolSize();

	void CutUseless();

	void Evolve();
	std::vector<Solution*> GetPool();
	void InitializePopulation();
private:

	int pool_min_size;

	double ratio;

	std::vector<Solution*> pool;


};




#endif // !_SolutionPool_H_





