#ifndef _MasterProblem_H_
#define _MasterProblem_H_


#include "gurobi_c++.h"
#include "SolutionPool.h"
#include "Callback.h"


class MasterProblem
{
public:
	MasterProblem(int n);
	~MasterProblem();


	void SetObjective(GRBModel& master_model);
	void SetAttributes(GRBModel& master_model);
	void SetParameters(GRBModel& master_model);
	void DefineVariables(GRBModel& master_model);
	void AddConstraints(GRBModel& master_model);

	void CreateInitialMP(GRBModel& master_mode);
	void AddRestriction(GRBModel& master_model, SolutionPool* sp);
	double SolveRMP(GRBModel& master_model);

	void PrintResult();

private:

	int num_N;

	std::string filename = datafile;


	double** time_empty = NULL;

	double** time_one = NULL;

	double** time_two = NULL;


	//create GRB variables
	GRBVar** x_Var = NULL;

	GRBVar** y_Var = NULL;

	GRBVar** u_Var = NULL;

	GRBVar** v_Var = NULL;

	GRBVar* S_Var = NULL;

	GRBVar** w_Var = NULL;

	GRBVar* t_Var = NULL;

};


#endif // !_MasterProblem_H_





