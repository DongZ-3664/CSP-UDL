#ifndef _InitialSolutionCreator_H_
#define _InitialSolutionCreator_H_

#include "Instance.h"
#include "Solution.h"


class InitialSolutionCreator
{
public:
	InitialSolutionCreator();

	~InitialSolutionCreator();

	void GreedyInsert(Solution*& SolPtr, Instance* ins, int runs);

	void CreatePopulation(std::vector<Solution*>& SolPop, Instance* ins, int runs);

private:

};

#endif // !_InitialSolution_H_

