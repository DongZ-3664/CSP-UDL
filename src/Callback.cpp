#include "Callback.h"

#define _PrintInfo_ 0

Callback::Callback(GRBVar** xx, GRBVar** yy, GRBVar** uu, GRBVar** vv, int nn)
{
	_X_Var = xx;
	_Y_Var = yy;
	_U_Var = uu;
	_V_Var = vv;

	nJobs = nn;
}

void Callback::ReSetMembers()
{

	_X_Value = new double* [nJobs + 1];
	_Y_Value = new double* [nJobs + 1];
	_U_Value = new double* [nJobs + 1];
	_V_Value = new double* [nJobs + 1];

	for (auto i = 0u; i <= nJobs; ++i)
	{
		_X_Value[i] = getSolution(_X_Var[i], nJobs + 2);
		_Y_Value[i] = getSolution(_Y_Var[i], nJobs + 1);
		_U_Value[i] = getSolution(_U_Var[i], nJobs + 1);
		_V_Value[i] = getSolution(_V_Var[i], nJobs + 1);
	}
}

Callback::~Callback()
{

}

void Callback::DeleteMembers()
{
	Release_02<double>(_X_Value, nJobs);
	Release_02<double>(_Y_Value, nJobs);
	Release_02<double>(_U_Value, nJobs);
	Release_02<double>(_V_Value, nJobs);
}

void Callback::callback()
{
	try {
		if (where == GRB_CB_MIPSOL) {

			ReSetMembers();

			int XTourLength = 0;
			int* XSuccessor = NULL;
			int MinXLoopStart = 0;
			int MinXLoopSize = 0;

			FindMinLoopX(XSuccessor, XTourLength, MinXLoopStart, MinXLoopSize);

#if _PrintInfo_
			std::cout << "call back x :" << std::endl;
			std::cout << "MinXLoopSize : " << MinXLoopSize << std::endl;
			std::cout << "XTourLength : " << XTourLength << std::endl;
#endif // _PrintInfo_

			if (MinXLoopSize < XTourLength)
			{
				std::ostringstream ostr;

				GRBLinExpr sum = 0;
				int i = MinXLoopStart;
				while (XSuccessor[i] != nJobs + 1 && XSuccessor[i] != MinXLoopStart)
				{
					sum += _X_Var[i][XSuccessor[i]];

					ostr << "x[" << i << "][" << XSuccessor[i] << "] +";

					i = XSuccessor[i];
				}

				sum += _X_Var[i][XSuccessor[i]];

				ostr << "x[" << i << "][" << XSuccessor[i] << "]";

				if (XSuccessor[i] == nJobs + 1)
				{	
					addLazy(sum <= MinXLoopSize - 2);

#if _PrintInfo_
					std::cout << "0add lazy : " << ostr.str() << " <= " << MinXLoopSize - 2 << std::endl;
#endif // _PrintInfo_	

				}
				else
				{
					addLazy(sum <= MinXLoopSize - 1);
				}

#if _PrintInfo_
				std::cout << "add lazy : " << ostr.str() << " <= " << MinXLoopSize - 1 << std::endl;
#endif // _PrintInfo_

			

				delete[] XSuccessor;
				XSuccessor = NULL;
				DeleteMembers();
				return;
			}
			delete[] XSuccessor;
			XSuccessor = NULL;


			int* YSuccessor = NULL;
			int MinYLoopSize = 0;
			int MinYLoopStart = 0;
			FindMinLoopY(YSuccessor, MinYLoopStart, MinYLoopSize);

			if (MinYLoopSize <= nJobs)
			{

#if _PrintInfo_
				std::cout << "find sub tour of var y " << std::endl;
#endif // _PrintInfo_

				GRBLinExpr sum = 0;
				std::ostringstream ostr;

				int cur = MinYLoopStart;
				while (YSuccessor[cur] != MinYLoopStart)
				{
					sum += _Y_Var[cur][YSuccessor[cur]];
					ostr << "y[" << cur << "][" << YSuccessor[cur] << "] +";
					cur = YSuccessor[cur];
				}

				ostr << "y[" << cur << "][" << YSuccessor[cur] << "]";

				addLazy(sum <= MinYLoopSize - 1);

#if _PrintInfo_
				std::cout << ostr.str() << " <= " << MinYLoopSize - 1 << std::endl;
#endif // _PrintInfo_

				


				delete[] YSuccessor;
				YSuccessor = NULL;
				DeleteMembers();
				return;
			}
			delete[] YSuccessor;
			YSuccessor = NULL;



			std::vector<std::vector<int>> UYVTour;
			FindTourUYV(UYVTour);

#if _PrintInfo_
			OutPutUV();
			for (auto s : UYVTour)
			{
				for (auto v : s)
				{
					std::cout << v << ", ";
				}
				std::cout << std::endl;
			}
#endif // _PrintInfo_

			for (auto tr : UYVTour)
			{
				if (tr[0] == tr.back()) continue;

#if _PrintInfo_
				std::cout << "find misplaced v : " << std::endl;
#endif // _PrintInfo_


				int len = tr.size();
				int i = 1;

				GRBLinExpr sum_y = 0;
				GRBLinExpr sum_y_tail = 0;

				std::ostringstream ostr;
				while (i < len - 2)
				{
					sum_y += _Y_Var[tr[i]][tr[i + 1]];
					ostr << "+ y[" << tr[i] << "][" << tr[i + 1] << "] ";
					++i;
				}

				for (int j = 1; j <= nJobs; ++j)
				{
					if (j == tr[len - 2]) continue;
					sum_y_tail += _Y_Var[tr[len - 2]][j];
				}

				addLazy(_V_Var[tr[len - 2]][tr[0]] >= _U_Var[tr[0]][tr[1]] + sum_y - (len - 3) - sum_y_tail);

#if _PrintInfo_
				std::cout << "add lazy : v[" << tr[len - 2] << "][" << tr[0] << "] >= u[" << tr[0] << "][" << tr[1] << "] ";
				std::cout << ostr.str() << " - " << (len - 3) << " - sum( y[" << tr[len - 2] << "][*] )" << std::endl;
#endif // _PrintInfo_

			}


			if (XSuccessor != NULL) { delete[] XSuccessor; XSuccessor = NULL; }
			if (YSuccessor != NULL) { delete[] YSuccessor; YSuccessor = NULL; }

			DeleteMembers();
		}
	}
	catch (GRBException e) {
		std::cerr << "Error number: " << e.getErrorCode() << std::endl;
		std::cerr << e.getMessage() << std::endl;
	}
	catch (...) {
		std::cerr << "Error during callback" << std::endl;
	}

}


void Callback::FindMinLoopY(int*& y_succ, int& start, int& min_size)
{
	if (y_succ != NULL) { delete[] y_succ; }
	y_succ = new int[nJobs + 1];
	bool* visited = new bool[nJobs + 1];

	for (int i = 1; i <= nJobs; ++i)
	{
		y_succ[i] = 0;

		visited[i] = false;

		for (int j = 1; j <= nJobs; ++j)
		{
			if (j == i) continue;

			if (this->_Y_Value[i][j] > 0.5)
			{
				y_succ[i] = j;
				break;
			}

		}
	}


	min_size = nJobs + 1;			//initialize min_size;
	start = 0;


	int cur_head = 0;
	int cur_tail = 0;
	int cur_size = 0;


	while (++cur_head <= nJobs)
	{

		if (y_succ[cur_head] == 0 || visited[cur_head]) continue;


		cur_tail = y_succ[cur_head];
		visited[cur_head] = true;
		visited[cur_tail] = true;
		cur_size = 1;


		while (y_succ[cur_tail] > 0 && cur_tail != cur_head)
		{
			cur_tail = y_succ[cur_tail];
			visited[cur_tail] = true;
			++cur_size;
		}

		if (cur_tail == cur_head && cur_size < min_size)
		{
			start = cur_head;
			min_size = cur_size;
			continue;
		}
	}

	delete[] visited; visited = NULL;
}

void Callback::FindTourUYV(std::vector<std::vector<int>>& uv_tour)
{
	uv_tour.clear();
	std::vector<std::vector<int>> temp_y;
	for (int i = 1; i <= nJobs; ++i)
	{
		for (int j = 1; j <= nJobs; ++j)
		{
			if (j == i) continue;
			if (_U_Value[i][j] > 0.5 && _V_Value[j][i] < 0.5)
			{
				uv_tour.push_back({ i,j });
			}

			if (_Y_Value[i][j] > 0.5)
			{
				temp_y.push_back({ i,j });
			}
		}
	}


	bool find = true;
	while (find)
	{
		find = false;
		for (int i = 0; i < uv_tour.size(); ++i)
		{
			for (int j = 0; j < temp_y.size(); ++j)
			{
				if (uv_tour[i].back() == temp_y[j][0])
				{
					uv_tour[i].push_back(temp_y[j][1]);
					find = true;
				}
			}
		}
	}



	for (int i = 0; i < uv_tour.size(); ++i)
	{
		for (int j = 1; j <= nJobs; ++j)
		{
			if (_V_Value[uv_tour[i].back()][j] < 0.5) continue;

			uv_tour[i].push_back(j);

		}
	}
}

void Callback::FindMinLoopX(int*& x_succ, int& xtoursize, int& start, int& min_size)
{
	if (x_succ != NULL) { delete[] x_succ; }
	x_succ = new int[nJobs + 1u];
	bool* visited = new bool[nJobs + 1u];
	xtoursize = 1;
	for (auto i = 0u; i <= nJobs; ++i)
	{
		x_succ[i] = 0;
		visited[i] = true;

		for (int j = 1; j <= nJobs + 1u; ++j)
		{
			if (j == i) continue;

			if (_X_Value[i][j] > 0.5)
			{
				x_succ[i] = j;
				visited[i] = false;
				++xtoursize;
				break;
			}

		}
	}

	min_size = xtoursize;			//initialize min_size;
	start = 0;


	int cur_head = 0;
	int cur_tail = 0;
	int cur_size = 0;


	while (cur_head <= nJobs)
	{
		if (visited[cur_head]) { ++cur_head; continue; }

		cur_tail = x_succ[cur_head];
		visited[cur_head] = true;

		cur_size = 1;


		while (cur_tail != nJobs + 1 && !visited[cur_tail])
		{
			visited[cur_tail] = true;

			cur_tail = x_succ[cur_tail];

			++cur_size;
		}


		if (cur_tail == nJobs + 1)
		{
			++cur_size;
		}

		if (cur_size < min_size)
		{
			start = cur_head;
			min_size = cur_size;
		}
		++cur_head;
	}

	delete[] visited; visited = NULL;
	return;
}

bool Callback::IsFeasible()
{
	DeleteMembers();

	_X_Value = new double* [nJobs + 1u];
	_Y_Value = new double* [nJobs + 1u];
	_U_Value = new double* [nJobs + 1u];
	_V_Value = new double* [nJobs + 1u];

	for (auto i = 0u; i <= nJobs; ++i)
	{
		_X_Value[i] = new double [nJobs + 2u];
		_Y_Value[i] = new double [nJobs + 1u];
		_U_Value[i] = new double [nJobs + 1u];
		_V_Value[i] = new double [nJobs + 1u];

		for (auto j = 0u; j <= nJobs; ++j)
		{
			_X_Value[i][j] = _X_Var[i][j].get(GRB_DoubleAttr_X);
			_Y_Value[i][j] = _Y_Var[i][j].get(GRB_DoubleAttr_X);
			_U_Value[i][j] = _U_Var[i][j].get(GRB_DoubleAttr_X);
			_V_Value[i][j] = _V_Var[i][j].get(GRB_DoubleAttr_X);

		}
		_X_Value[i][nJobs + 1u] = _X_Var[i][nJobs + 1u].get(GRB_DoubleAttr_X);

	}

	
	int* YSuccessor = NULL;
	int MinYLoopSize = 0;
	int MinYLoopStart = 0;
	FindMinLoopY(YSuccessor, MinYLoopStart, MinYLoopSize);

	if (MinYLoopSize <= nJobs)
	{
		std::cout << "find sub tour of var y :" << std::endl;

		GRBLinExpr sum = 0;
		std::ostringstream ostr;
		int cur = MinYLoopStart;
		while (YSuccessor[cur] != MinYLoopStart)
		{
			sum += _Y_Var[cur][YSuccessor[cur]];
			ostr << "y[" << cur << "][" << YSuccessor[cur] << "] ";
			cur = YSuccessor[cur];
		}
		std::cout << ostr.str() << std::endl;

		delete[] YSuccessor; 
		YSuccessor = NULL;
		DeleteMembers();

		return false;
	}
	



	int XTourLength = 0;
	int* XSuccessor = NULL;
	int MinXLoopStart = 0;
	int MinXLoopSize = 0;

	FindMinLoopX(XSuccessor, XTourLength, MinXLoopStart, MinXLoopSize);

	std::cout << "MinXLoopSize : " << MinXLoopSize << std::endl;
	std::cout << "XTourLength : " << XTourLength << std::endl;
	std::ostringstream ostr;

	GRBLinExpr sum = 0;
	int i = MinXLoopStart;
	while (XSuccessor[i] != nJobs + 1 && XSuccessor[i] != MinXLoopStart)
	{
		sum += _X_Var[i][XSuccessor[i]];
		ostr << "x[" << i << "][" << XSuccessor[i] << "] ";

		i = XSuccessor[i];

	}

	ostr << "x[" << i << "][" << XSuccessor[i] << "]";

	std::cout << ostr.str() << std::endl;

	if (MinXLoopSize < XTourLength)
	{
		delete[] XSuccessor;
		XSuccessor = NULL;
		DeleteMembers();
		return false;
	}


	std::vector<std::vector<int>> UYVTour;
	FindTourUYV(UYVTour);

	//OutPutUV();

	for (auto s : UYVTour)
	{
		for (auto v : s)
		{
			std::cout << v << ", ";
		}
		std::cout << std::endl;
	}

	for (auto tr : UYVTour)
	{
		if (tr[0] == tr.back()) continue;

		std::cout << "find misplaced v : " << std::endl;
		for (auto s : tr)
		{
			std::cout << s << ", ";
		}
		std::cout << std::endl;

		DeleteMembers();
		return false;
	}




	if (YSuccessor != NULL) { delete[] YSuccessor; YSuccessor = NULL; }
	if (XSuccessor != NULL) { delete[] XSuccessor; XSuccessor = NULL; }

	DeleteMembers();
	
	return true;
}

void Callback::FindSubTourX(int& tourlength, int*& xtour)
{
	bool* visited = new bool[nJobs + 2];

	int x_tour_size = 2;        // 0 & N + 1
	for (auto i = 1u; i <= nJobs; ++i)
	{
		visited[i] = true;
		for (auto j = 1u; j <= nJobs + 1; ++j)
		{
			if (_X_Value[i][j] > 0.5)
			{
				visited[i] = false;
				++x_tour_size;
				break;
			}
		}
	}
	visited[nJobs + 1] = false;

	xtour = new int[x_tour_size];
	xtour[0] = 0;
	visited[0] = true;

	int cur_tail = 0;
	while (cur_tail < x_tour_size - 1)          //[0,....,x_tour_size - 1] x_tour_size
	{
		bool find_next = false;

		for (auto i = 1; i <= this->nJobs + 1; ++i)
		{
			if (!visited[i] && _X_Value[xtour[cur_tail]][i] > 0.5)
			{
				xtour[++cur_tail] = i;
				visited[i] = true;
				find_next = true;
				break;
			}
		}

		if (!find_next || xtour[cur_tail] == nJobs + 1)
		{
			int next_start = 0;

			while (visited[next_start])
			{
				++next_start;
			}

			xtour[++cur_tail] = next_start;
			visited[next_start] = true;
		}
	}



	tourlength = x_tour_size;
#ifndef NDEBUG
	for (auto i = 0; i < x_tour_size; ++i)
	{
		std::cout << xtour[i] << ", ";
	}
	std::cout << std::endl;
#endif // !NDEBUG


	delete[] visited;
	visited = NULL;

}

void Callback::OutPutUV()
{
#ifndef NDEBUG


	for (int i = 0; i <= nJobs; ++i)
	{
		for (int j = 0; j <= nJobs; ++j)
		{
			if (_U_Value[i][j] > 0.5)
			{
				std::cout << "u[" << i << "][" << j << "] = " << _U_Value[i][j] << std::endl;
			}
		}
	}
	std::cerr << "-----------------------------------" << std::endl;

	for (int i = 0; i <= nJobs; ++i)
	{
		for (int j = 0; j <= nJobs; ++j)
		{
			if (_V_Value[i][j] > 0.5)
			{
				std::cout << "v[" << i << "][" << j << "] = " << _V_Value[i][j] << std::endl;
			}
		}
	}
	std::cerr << "-----------------------------------" << std::endl;

	for (int i = 0; i <= nJobs; ++i)
	{
		for (int j = 0; j <= nJobs; ++j)
		{
			if (_Y_Value[i][j] > 0.5)
			{
				std::cout << "y[" << i << "][" << j << "] = " << _Y_Value[i][j] << std::endl;
			}
		}
	}
	std::cerr << "-----------------------------------" << std::endl;

#endif // !NDEBUG
	return;

}


void Callback::GetPath(int*& path)
{
	DeleteMembers();

	_X_Value = new double* [nJobs + 1u];
	_Y_Value = new double* [nJobs + 1u];
	_U_Value = new double* [nJobs + 1u];
	_V_Value = new double* [nJobs + 1u];

	for (auto i = 0u; i <= nJobs; ++i)
	{
		_X_Value[i] = new double[nJobs + 2u];
		_Y_Value[i] = new double[nJobs + 1u];
		_U_Value[i] = new double[nJobs + 1u];
		_V_Value[i] = new double[nJobs + 1u];

		for (auto j = 0u; j <= nJobs; ++j)
		{
			_X_Value[i][j] = _X_Var[i][j].get(GRB_DoubleAttr_X);
			_Y_Value[i][j] = _Y_Var[i][j].get(GRB_DoubleAttr_X);
			_U_Value[i][j] = _U_Var[i][j].get(GRB_DoubleAttr_X);
			_V_Value[i][j] = _V_Var[i][j].get(GRB_DoubleAttr_X);

		}
		_X_Value[i][nJobs + 1u] = _X_Var[i][nJobs + 1u].get(GRB_DoubleAttr_X);

	}


	path = new int[2 * nJobs + 2];

	path[0] = 0;
	int tail = 0;
	int end_job = 0;

	while (tail < 2 * nJobs)
	{
		for (int i = 1; i <= nJobs; ++i)
		{
			if (_X_Value[end_job][i] < 0.5) continue;

			end_job = i;
			path[++tail] = i;
			path[++tail] = nJobs + i;

			for (int j = 1; j <= nJobs; ++j)
			{
				if (_U_Value[i][j] > 0.5)
				{
					path[tail] = j;
					path[++tail] = nJobs + j;

					bool find_y = true;
					while (find_y)
					{
						find_y = false;
						for (int k = 1; k <= nJobs; ++k)
						{
							if (_Y_Value[path[tail - 1]][k] < 0.5) continue;

							path[++tail] = k;
							path[++tail] = nJobs + k;

							find_y = true;
						}

					}

					for (int l = 1; l <= nJobs; l++)
					{
						if (_V_Value[path[tail - 1]][l] > 0.5)
						{
							if (l != end_job) { std::cout << "error!!!"; return; }

							path[++tail] = end_job + nJobs;
							break;
						}

					}
				}

			}
		}

	}

	return;
}
