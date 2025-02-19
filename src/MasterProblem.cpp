#include "MasterProblem.h"

MasterProblem::MasterProblem(int n)
{
	num_N = n;

}

MasterProblem::~MasterProblem()
{
	Release_02<double>(time_empty, 2 * num_N);
	Release_02<double>(time_one, 2 * num_N);
	Release_02<double>(time_two, 2 * num_N);

	Release_02<GRBVar>(x_Var, num_N);
	Release_02<GRBVar>(y_Var, num_N);
	Release_02<GRBVar>(u_Var, num_N);
	Release_02<GRBVar>(v_Var, num_N);
	Release_01<GRBVar>(S_Var);

#if (_Callback_)

#else

	Release_01<GRBVar>(t_Var);

#endif	//(_Callback_)
}

void MasterProblem::SetParameters(GRBModel& master_model)
{
	
	//double** time_empty;
	CreateArray_02<double>(time_empty, 2 * num_N, 2 * num_N);
	ReadData_02(filename, time_empty, 2 * num_N, 2 * num_N);


	//double** time_one;
	CreateArray_02<double>(time_one, 2 * num_N, 2 * num_N);
	for (int i = 1; i <= 2 * num_N; i++)
		for (int j = 1; j <= 2 * num_N; j++)
			time_one[i][j] = Multiplier_L1 * time_empty[i][j];

	//double** time_two;
	CreateArray_02<double>(time_two, 2 * num_N, 2 * num_N);
	for (int i = 1; i <= 2 * num_N; i++)
		for (int j = 1; j <= 2 * num_N; j++)
			time_two[i][j] = Multiplier_L2 * time_empty[i][j];

}


void MasterProblem::DefineVariables(GRBModel& master_model)
{
	//GRBVar** x_Var;
	CreateArray_02<GRBVar>(x_Var, num_N, num_N + 1);
	for (int i = 0; i <= num_N; i++)
		for (int j = 0; j <= num_N + 1; j++)
			x_Var[i][j] = master_model.addVar(0.0, 1.0, 0.0, GRB_BINARY, "x[" + itos(i) + "][" + itos(j) + "]");

	//GRBVar** y_Var;
	CreateArray_02<GRBVar>(y_Var, num_N, num_N);
	for (int i = 0; i <= num_N; i++)
		for (int j = 0; j <= num_N; j++)
			y_Var[i][j] = master_model.addVar(0.0, 1.0, 0.0, GRB_BINARY, "y[" + itos(i) + "][" + itos(j) + "]");

	//GRBVar** u_Var;
	CreateArray_02<GRBVar>(u_Var, num_N, num_N);
	for (int i = 0; i <= num_N; i++)
		for (int j = 0; j <= num_N; j++)
			u_Var[i][j] = master_model.addVar(0.0, 1.0, 0.0, GRB_BINARY, "u[" + itos(i) + "][" + itos(j) + "]");

	//GRBVar** v_Var;
	CreateArray_02<GRBVar>(v_Var, num_N, num_N);
	for (int i = 0; i <= num_N; i++)
		for (int j = 0; j <= num_N; j++)
			v_Var[i][j] = master_model.addVar(0.0, 1.0, 0.0, GRB_BINARY, "v[" + itos(i) + "][" + itos(j) + "]");


	//GRBVar* S_Var;
	CreateArray_01<GRBVar>(S_Var, 2 * num_N + 2);
	for (int i = 0; i <= 2 * num_N + 2; i++)
		S_Var[i] = master_model.addVar(0.0, (2 * num_N + 1), 0.0, GRB_CONTINUOUS, "S[" + itos(i) + "]");


	//GRBVar** w_Var;
	CreateArray_02<GRBVar>(w_Var, num_N, num_N);
	for (int i = 0; i <= num_N; i++)
		for (int j = 0; j <= num_N; j++)
			w_Var[i][j] = master_model.addVar(0.0, 1.0, 0.0, GRB_BINARY, "w[" + itos(i) + "][" + itos(j) + "]");

#if _Callback_

#else
	CreateArray_01<GRBVar>(t_Var, num_N + 1);
	for (int i = 0; i <= num_N; i++)
		t_Var[i] = master_model.addVar(0.0, (num_N), 0.0, GRB_CONTINUOUS, "t[" + itos(i) + "]");

#endif // _Callback_

}



void MasterProblem::SetObjective(GRBModel& master_model)
{
	GRBLinExpr sum_obj = 0;
	for (int i = 1; i <= num_N; i++)
	{
		GRBLinExpr sum_1 = 0;
		for (int j = 1; j <= num_N; j++)
		{
			if (j == i) continue;
			sum_1 += u_Var[i][j];
		}
		sum_obj += (1 - sum_1) * time_one[i][num_N + i];


		for (int j = 1; j <= num_N; j++)
		{
			if (j == i) continue;

			sum_obj += time_empty[num_N + i][j] * x_Var[i][j];

			sum_obj += ((time_two[i][num_N + i] - time_one[i][num_N + i]) * (w_Var[j][i]));

			sum_obj += time_one[num_N + i][j] * y_Var[i][j];

			sum_obj += time_one[i][j] * u_Var[i][j];

			sum_obj += time_one[num_N + i][num_N + j] * v_Var[i][j];
		}
	}

	master_model.setObjective(sum_obj, GRB_MINIMIZE);
}


void MasterProblem::SetAttributes(GRBModel& master_model)
{
	master_model.set(GRB_IntParam_Threads, _RMP_MAX_THREADS);
	master_model.set(GRB_DoubleParam_TimeLimit, _RMP_MAX_TIME);

#if _Callback_

	master_model.set(GRB_IntParam_LazyConstraints, 1);

#endif

}


void MasterProblem::AddConstraints(GRBModel& master_model)
{
	//constraint 1 - 1 && 1 - 2
	for (int i = 1; i <= num_N; i++)
	{
		GRBLinExpr sum_1 = 0;
		GRBLinExpr sum_2 = 0;

		for (int j = 1; j <= num_N; j++)
		{
			if (i == j) continue;
			sum_1 += (x_Var[i][j] + y_Var[i][j] + v_Var[i][j]);
			sum_2 += (x_Var[j][i] + y_Var[j][i] + u_Var[j][i]);

		}
		sum_1 += x_Var[i][num_N + 1];
		sum_2 += x_Var[0][i];

		master_model.addConstr(sum_1 == 1);												//constraint 1 - 1
		master_model.addConstr(sum_2 == 1);												//constraint 1 - 2
	}


	//constraint 1 - 3
	{
		GRBLinExpr m_sum_01 = 0, m_sum_02 = 0;
		for (int i = 1; i <= num_N; i++)
		{
			m_sum_01 += x_Var[0][i];
			m_sum_02 += x_Var[i][num_N + 1];
		}
		master_model.addConstr(m_sum_01 == 1);												//constraint 1 - 3
		master_model.addConstr(m_sum_02 == 1);
	}


	//constraint 2
	for (int i = 1; i <= num_N; i++)
	{
		GRBLinExpr sum_1 = 0;
		GRBLinExpr sum_2 = 0;
		GRBLinExpr sum_3 = 0;
		GRBLinExpr sum_4 = 0;


		for (int j = 1; j <= num_N; j++)
		{
			if (j == i) continue;

			sum_1 += x_Var[i][j];
			sum_2 += x_Var[j][i];

			sum_3 += u_Var[i][j];
			sum_4 += v_Var[j][i];
		}

		sum_1 += x_Var[i][num_N + 1];
		sum_2 += x_Var[0][i];

		master_model.addConstr(sum_1 == sum_2);			//constraint 2
		master_model.addConstr(sum_3 == sum_4);			//constraint 3
		master_model.addConstr(sum_3 <= sum_2);			//constraint 4 - 1
		master_model.addConstr(sum_4 <= sum_1);			//constraint 4 - 2

	}

	for (int i = 1; i <= num_N; i++)
	{
		GRBLinExpr sum = 0;

		for (int j = 1; j <= num_N; j++)
		{
			if (j == i)	continue;

			sum += (w_Var[j][i] + u_Var[i][j]);

			GRBLinExpr sum_1 = 0;
			GRBLinExpr sum_3 = 0;
			GRBLinExpr sum_4 = 0;

			for (int k = 1; k <= num_N; k++)
			{
				if (k == j || k == i) continue;

				sum_1 += w_Var[i][k];

				sum_3 += y_Var[k][j];
				sum_4 += y_Var[j][k];
			}

			master_model.addConstr(u_Var[i][j] <= w_Var[i][j]);			//constraint 10 - 1
			master_model.addConstr(v_Var[j][i] <= w_Var[i][j]);			//constraint 10 - 2

			//model.addConstr(w_Var[i][j] <= u_Var[i][j] + sum_1);	//constraint 6 - 1
			//model.addConstr(w_Var[i][j] <= v_Var[j][i] + sum_1);	//constraint 6 - 2

			master_model.addConstr(w_Var[i][j] <= u_Var[i][j] + sum_3);	//constraint 7 - 1
			master_model.addConstr(w_Var[i][j] <= v_Var[j][i] + sum_4);	//constraint 7 - 2

		}

		master_model.addConstr(sum <= 1);									//constraint 8
	}


	for (int i = 1; i <= num_N; i++)
	{

		for (int j = 1; j <= num_N; j++)
		{
			if (j == i)	continue;

#if _N3_Constraints_

			for (int k = 1; k <= num_N; k++)
			{
				if (k == j || k == i) continue;
				master_model.addConstr(w_Var[k][i] <= w_Var[k][j] + 1 - (y_Var[i][j] + y_Var[j][i]));
			}

#else

			GRBLinExpr sum_1 = 0;
			GRBLinExpr sum_2 = 0;

			for (int k = 1; k <= num_N; k++)
			{
				if (k == j || k == i) continue;

				sum_1 += (k * w_Var[k][i]);
				sum_2 += (k * w_Var[k][j]);
			}

			master_model.addConstr(sum_1 <= sum_2 + (num_N) * (1 - y_Var[i][j] - y_Var[j][i]));

#endif	//_N3_Constraints_

		}
	}



	//constraint 5 - 1 & 5 - 2
	for (int i = 1; i <= num_N; i++)
	{
		for (int j = 1; j <= num_N; j++)
		{
			if (j == i)	continue;
			GRBLinExpr sum_1 = 0;
			GRBLinExpr sum_2 = 0;

			for (int k = 1; k <= num_N; k++)
			{
				if (k == j || k == i) continue;

				sum_1 += y_Var[i][k];
				sum_2 += y_Var[k][i];
			}

			master_model.addConstr(u_Var[j][i] <= v_Var[i][j] + sum_1);					//constraint 5 - 1
			master_model.addConstr(v_Var[i][j] <= u_Var[j][i] + sum_2);					//constraint 5 - 2
		}
	}


	for (int i = 1; i <= num_N; i++)
	{
		GRBLinExpr sum_1 = 0;
		GRBLinExpr sum_2 = 0;
		GRBLinExpr sum_3 = 0;

		for (int j = 1; j <= num_N; j++)
		{
			if (j == i) continue;
			sum_1 += x_Var[j][i];
			sum_2 += x_Var[i][j];

			sum_3 += w_Var[j][i];
		}

		master_model.addConstr(sum_1 + x_Var[0][i] + sum_3 <= 1);
		master_model.addConstr(sum_2 + x_Var[i][num_N + 1] + sum_3 <= 1);

	}

	
#if _Callback_


#else

	//M T Z - constraint  
	S_Var[0u].set(GRB_DoubleAttr_UB, 0);											//Subtour elimination 1
	S_Var[2 * num_N + 1].set(GRB_DoubleAttr_LB, 2 * num_N + 1);

	for (int i = 1; i <= num_N; i++)
	{
		GRBLinExpr sum_1 = 0;
		GRBLinExpr sum_2 = 0;

		for (int j = 1; j <= num_N; j++)
		{
			if (j == i) continue;
			sum_1 += 2 * u_Var[i][j];
			sum_2 += 2 * w_Var[i][j];

		}

		master_model.addConstr(x_Var[0][i] <= S_Var[i]);									//Subtour elimination 2
		master_model.addConstr(S_Var[num_N + i] + 1 <= S_Var[2 * num_N + 1]);				//Subtour elimination 3
		master_model.addConstr(S_Var[i] + 1 + sum_1 <= S_Var[num_N + i]);					//Subtour elimination 4 - 1

		//model.addConstr(S_Var[i] + 1 + sum_2 <= S_Var[num_N + i]);					//Subtour elimination 4 - 2

	}


	for (int i = 1; i <= num_N; i++)												//Subtour elimination 5 && 6 && 7
	{
		for (int j = 1; j <= num_N; j++)
		{
			if (j == i) continue;
			master_model.addConstr(S_Var[num_N + i] + 1 <= S_Var[j] + (2 * num_N) * (1 - x_Var[i][j] - y_Var[i][j]));
			master_model.addConstr(S_Var[i] + 1 <= S_Var[j] + (2 * num_N - 1) * (1 - u_Var[i][j]));
			master_model.addConstr(S_Var[num_N + i] + 1 <= S_Var[num_N + j] + (2 * num_N - 1) * (1 - v_Var[i][j]));
		}
	}


	for (int i = 1; i <= num_N; i++)
	{
		GRBLinExpr sum = 0;

		for (int j = 1; j <= num_N; j++)
		{
			if (j == i) continue;
			sum += (w_Var[j][i]);
		}
		master_model.addConstr(S_Var[i] == 2 * t_Var[i] + sum - 1);						//Subtour elimination 8
	}

#endif	//(_Callback_)

}



void MasterProblem::AddRestriction(GRBModel& master_model, SolutionPool* sp)
{
	int** CountX = NULL;
	int** CountY = NULL;
	int** CountW = NULL;

	int restriction = 0;
	sp->GetFixedVariables(CountX, CountY, CountW, num_N);


	//std::cerr << "solution pool size : " << sp->gets << std::endl;

	for (auto i = 1u; i <= num_N; ++i)
	{
		if (CountX[0][i] == sp->GetPoolSize())
		{
			x_Var[0][i].set(GRB_DoubleAttr_LB, 1.0);
			++restriction;
			break;
		}

		if (CountX[num_N + i][2 * num_N + 1] == sp->GetPoolSize())
		{
			x_Var[i][num_N + 1].set(GRB_DoubleAttr_LB, 1.0);

			++restriction;
			break;
		}
	}


	for (auto i = 1u; i <= num_N; ++i)
	{
		for (auto j = 1u; j <= num_N; ++j)
		{
			if (CountX[i][j] == sp->GetPoolSize())
			{
				u_Var[i][j].set(GRB_DoubleAttr_LB, 1.0);

				++restriction;
				break;
			}

			if (CountX[num_N + i][num_N + j] == sp->GetPoolSize())
			{
				v_Var[i][j].set(GRB_DoubleAttr_LB, 1.0);

				++restriction;
				break;
			}
		}
	}



	for (auto i = 1u; i <= num_N; ++i)
	{
		for (auto j = 1u; j <= num_N; ++j)
		{
			if (CountX[num_N + i][j] == sp->GetPoolSize())
			{
				x_Var[i][j].set(GRB_DoubleAttr_LB, 1.0);

				++restriction;
				break;
			}

			if (CountY[num_N + i][j] == sp->GetPoolSize())
			{
				y_Var[i][j].set(GRB_DoubleAttr_LB, 1.0);
				++restriction;
				break;
			}

		}
	}




	//  w restrictions
	for (auto i = 1u; i <= num_N; ++i) {
		for (auto j = 1u; j <= num_N; ++j) {

			if (CountW[i][j] == 0) {
				w_Var[i][j].set(GRB_DoubleAttr_UB, 0.0);
				++restriction;
			}
			else if (CountW[i][j] == sp->GetPoolSize()) {
				w_Var[i][j].set(GRB_DoubleAttr_LB, 1.0);
				++restriction;
			}
		
		}
	}


	std::cout << "The number of restrictions : " << restriction << std::endl;

	Release_02<int>(CountX, 2 * num_N);
	Release_02<int>(CountY, 2 * num_N);
}


void MasterProblem::CreateInitialMP(GRBModel& master_mode)
{
	SetAttributes(master_mode);
	SetParameters(master_mode);
	DefineVariables(master_mode);
	SetObjective(master_mode);
	AddConstraints(master_mode);
}


double MasterProblem::SolveRMP(GRBModel& master_model)
{
	Callback cb(x_Var, y_Var, u_Var, v_Var, num_N);
#if _Callback_
	master_model.setCallback(&cb);

#endif	//(_Callback_)

	master_model.optimize();
	std::cout << " print path : " << std::endl;
	int* sol_path = NULL;
	cb.GetPath(sol_path);

	for (int i = 0; i < 2 * num_N; ++i)
	{
		std::cout << sol_path[i] << " ";
	}
	std::cout << std::endl;

	delete[] sol_path;

	this->PrintResult();
	return master_model.get(GRB_DoubleAttr_ObjVal);
}


void MasterProblem::PrintResult()
{

	for (int i = 0; i <= num_N; i++)
	{
		for (int j = 1; j <= num_N + 1; j++)
		{
			if (j != i && x_Var[i][j].get(GRB_DoubleAttr_X) > 0.5)
			{
				std::cerr << x_Var[i][j].get(GRB_StringAttr_VarName) << " = "
					<< x_Var[i][j].get(GRB_DoubleAttr_X) << std::endl;
			}
		}
	}

	std::cerr << "-----------------------------------" << std::endl;
	for (int i = 1; i <= num_N; i++)
	{
		for (int j = 1; j <= num_N; j++)
		{
			if (j != i && u_Var[i][j].get(GRB_DoubleAttr_X) > 0.5)
			{
				std::cerr << u_Var[i][j].get(GRB_StringAttr_VarName) << " = "
					<< u_Var[i][j].get(GRB_DoubleAttr_X) << std::endl;
			}
		}
	}

	std::cerr << "-----------------------------------" << std::endl;
	for (int i = 1; i <= num_N; i++)
	{
		for (int j = 1; j <= num_N; j++)
		{
			if (j != i && v_Var[i][j].get(GRB_DoubleAttr_X) > 0.5)
			{
				std::cerr << v_Var[i][j].get(GRB_StringAttr_VarName) << " = "
					<< v_Var[i][j].get(GRB_DoubleAttr_X) << std::endl;
			}
		}
	}

	std::cerr << "-----------------------------------" << std::endl;

	for (int i = 1; i <= num_N; i++)
	{
		for (int j = 1; j <= num_N; j++)
		{
			if (j != i && y_Var[i][j].get(GRB_DoubleAttr_X) > 0.5)
			{
				std::cerr << y_Var[i][j].get(GRB_StringAttr_VarName) << " = "
					<< y_Var[i][j].get(GRB_DoubleAttr_X) << std::endl;
			}
		}
	}

	std::cerr << "-----------------------------------" << std::endl;

}


