#include "RelocateMethod.h"

extern bool cmp1(const Record& IP1, const Record& IP2);

// Member function of class RelocateMethod
RelocateMethod::RelocateMethod(std::string name)
{
	this->OperatorName = name;
}

RelocateMethod::~RelocateMethod()
{
	ClearQueue();			
}


void RelocateMethod::ClearQueue()
{
	while (!this->Candidatequeue.empty())
	{
		this->Candidatequeue.pop();
	} 
	
	return;
}


void RelocateMethod::PrintCandidateQueue()
{
	std::cerr << "--------------";
	std::cerr << "The information of current candidate queue";
	std::cerr << "--------------" << std::endl;

	if (this->Candidatequeue.empty())
	{
		std::cerr << "Current candidate queue is empty !" << std::endl;
		return;
	}

	std::priority_queue<CandidateCouple, std::vector<CandidateCouple>, cmp3> copy_queue;
	copy_queue = this->Candidatequeue;
	while (!copy_queue.empty())
	{
		CandidateCouple cp_top = copy_queue.top();
		std::cerr << "To be inserted couple {" << cp_top.request_id << "}" << std::endl;
		std::cerr << "rm cost : " << cp_top.RmCost << std::endl;
		std::cerr << "records : " << cp_top.record_vec.size() << std::endl;

		while(!cp_top.record_vec.empty())
		{
			Record copy_suc = cp_top.record_vec.back();
			std::cerr << "Insert before node : " << copy_suc.p_d_successor[0] << " , "
				      << copy_suc.p_d_successor[1];

			std::cerr << "  in cost : " << copy_suc.InCost << " ";
			std::cerr << "  Regert value : " << copy_suc.RegretValue << std::endl;

			cp_top.record_vec.pop_back();
		}
		copy_queue.pop();
	}
}


int RelocateMethod::GetCandidateQueueSize()
{
	return this->Candidatequeue.size();
}

std::vector<int> RelocateMethod::GetMoveJobs()
{
	return this->mv_job_vec;
}

bool  RelocateMethod::CanBeRemoved(Solution* sol, int p_pos, int d_pos)
{
	if (p_pos > d_pos)
	{
		return false;
	}

	for (int i = p_pos; i <= d_pos; ++i)
	{
		if (_Rm_Flag_of_node[sol->NodeAt(i)] == false)
			return false;
	}
	return true;
}

bool RelocateMethod::CanBeInserted(Solution* sol, int p_pos, int d_pos)
{
	if (p_pos > d_pos)
	{
		return false;
	}

	for (int i = p_pos; i <= d_pos; ++i)
	{
		if (_In_Flag_of_node[sol->NodeAt(i)] == false)
			return false;
	}
	return true;
}

void RelocateMethod::ForbidRmove_Range(Solution* sol, int start_pos, int end_pos)
{
	if (start_pos == -1)
	{
		++start_pos;
	}
	for (int i = start_pos; i <= end_pos; i++)
	{
		_Rm_Flag_of_node[sol->NodeAt(i)] = false;
	}
}

void RelocateMethod::ForbidRmove_Unit(Solution* sol, int pos)
{
	_Rm_Flag_of_node[sol->NodeAt(pos)] = false;
}

void RelocateMethod::ForbidInsert_Range(Solution* sol, int start_pos, int end_pos)
{
	if (start_pos == -1)
	{
		++start_pos;
	}

	for (int i = start_pos; i <= end_pos; i++)
	{
		_In_Flag_of_node[sol->NodeAt(i)] = false;
	}
}

void RelocateMethod::ForbidInsert_Unit(Solution* sol, int pos)
{
	_In_Flag_of_node[sol->NodeAt(pos)] == false;
}



std::vector<Record> RelocateMethod::GainRecordQueOFJob(Solution* sol, int job_id, double cost_rm)
{
	std::vector<Record> RecordQueue;

	for (auto p_in = 0; p_in <= sol->GetNbNodes(); ++p_in)
	{
		std::vector<int> d_in_vec = sol->GetDInsertedPos(p_in);

		for (auto d_in : d_in_vec)
		{
			double cost_in = sol->GainInertionAdd(job_id, p_in, d_in);
			if (cost_rm > cost_in + 0.1)
			{
				RecordQueue.emplace_back(Record(sol->NodeAt(p_in), sol->NodeAt(d_in), cost_in, 0.0));			
			}
			else { break; }	//Enforce break need to be verified !!!
		}
	}

	if (RecordQueue.empty()) { return RecordQueue; }

	//设置RecordQueue最大容量为5,删除多余部分
	//std::sort(RecordQueue.begin(), RecordQueue.end(), cmp1);	

	//和直接sort无明显差异
	if (RecordQueue.size() <= 32)
	{
		std::sort(RecordQueue.begin(), RecordQueue.end(), cmp1);
	}
	else
	{
		int n = 1;
		while (n <= 5)
		{		
			int tail = RecordQueue.size() - n;
			int index = tail;
			double min_in = RecordQueue[tail].InCost;
			for (int i = 0; i <= tail; ++i)
			{
				if (RecordQueue[i].InCost < min_in)
				{
					index = i;
					min_in = RecordQueue[i].InCost;
				}
			}

			std::swap(RecordQueue[index], RecordQueue[tail]);
			++n;
		}

	}


	if (RecordQueue.size() > 5)
	{
		RecordQueue.erase(RecordQueue.begin(), RecordQueue.end() - 5);
	}

	RecordQueue[0].RegretValue = cost_rm - RecordQueue[0].InCost;
	for (auto i = 1u; i < RecordQueue.size(); ++i)
	{
		RecordQueue[i].RegretValue = RecordQueue[i - 1].InCost - RecordQueue[i].InCost;
	}

	return RecordQueue;
}


void RelocateMethod::GenerateCandidateQue(Solution* sol, TabuOperatorList* tbls)
{
	std::vector<int> job_set = sol->GetInstance()->JobSet;

	std::srand(time(NULL));
	std::random_shuffle(job_set.begin(), job_set.end());
	int N = sol->GetNbJobs();

	//traversal job set
	for (auto& x_p : job_set)
	{
		if (tbls->Find(x_p))
		{
			continue;
		}

		Solution* tp_sol = new Solution(sol);

		int x_d = x_p + N;
		int Rx_p_pos = _pos_of_node[x_p];
		int Rx_d_pos = _pos_of_node[x_d];


		double CostRm = tp_sol->GainRemovalSub(x_p, Rx_p_pos, Rx_d_pos);

		//Remove job
		tp_sol->RemoveLoad(Rx_p_pos, Rx_d_pos);
		tp_sol->RemoveNodeAtPosition(Rx_d_pos);
		tp_sol->RemoveNodeAtPosition(Rx_p_pos);
		tp_sol->ChangeCost(-CostRm);


		//Create a priority queue that stores the insertion position
		std::vector<Record> RecordQueue = GainRecordQueOFJob(tp_sol, x_p, CostRm);


		//create a record corresponding to the request x
		if (!RecordQueue.empty())
		{
			this->Candidatequeue.emplace(CandidateCouple(x_p, RecordQueue, CostRm));
		}





		delete tp_sol;
	}
}




void RelocateMethod::RelocateMultiCouple(Solution*& sol)
{
	int N = sol->GetNbJobs();

	//Initialize RemovableFlag and InsertionFlag;
	if (_In_Flag_of_node != NULL) { delete[] _In_Flag_of_node; }
	if (_Rm_Flag_of_node != NULL) { delete[] _Rm_Flag_of_node; }

	_In_Flag_of_node = new bool[2 * N + 2];
	_Rm_Flag_of_node = new bool[2 * N + 2];
	_In_Flag_of_node[0] = false;
	_Rm_Flag_of_node[0] = false;
	for (auto i = 1; i < 2 * N + 2; ++i)
	{
		_In_Flag_of_node[i] = true;
		_Rm_Flag_of_node[i] = true;
	}

	while (!this->Candidatequeue.empty())
	{
		//PrintCandidateQueue();

		CandidateCouple current_couple = this->Candidatequeue.top();
		this->Candidatequeue.pop();
		int x_p = current_couple.request_id;
		int x_d = x_p + N;

		int Rx_p_pos = _pos_of_node[x_p];
		int Rx_d_pos = _pos_of_node[x_d];


		//Before removing, verify the tow position can be removed;
		if (this->CanBeRemoved(sol, Rx_p_pos, Rx_d_pos))
		{
			double rm_cost = current_couple.RmCost;

			//移除job之前要维护_pos_of_node
			for (auto i = Rx_p_pos; i < Rx_d_pos; ++i)
			{
				--_pos_of_node[sol->NodeAt(i)];
			}
			for (auto i = Rx_d_pos + 1; i <= 2 * N; ++i)
			{
				_pos_of_node[sol->NodeAt(i)] -= 2;
			}

			_pos_of_node[x_p] = -1;
			_pos_of_node[x_d] = -1;

			sol->RemoveLoad(Rx_p_pos, Rx_d_pos);
			sol->RemoveNodeAtPosition(Rx_d_pos);
			sol->RemoveNodeAtPosition(Rx_p_pos);
			sol->ChangeCost(-rm_cost);

			Record current_record = current_couple.record_vec.back();
			current_couple.record_vec.pop_back();


			int p_in_pos = _pos_of_node[current_record.p_d_successor[0]];
			int d_in_pos = _pos_of_node[current_record.p_d_successor[1]];

			if (this->CanBeInserted(sol, p_in_pos, d_in_pos))
			{
				//移除之后更改flag，而且要在判断可以插入之后再更改；
				ForbidRmove_Range(sol, Rx_p_pos - 1, Rx_d_pos - 1);
				ForbidInsert_Range(sol, Rx_p_pos, Rx_d_pos - 1);

				ForbidInsert_Range(sol, p_in_pos, d_in_pos);
				_In_Flag_of_node[x_d] = false;
				_Rm_Flag_of_node[x_d] = false;
				_In_Flag_of_node[x_p] = false;
				_Rm_Flag_of_node[x_p] = false;


				//维护_pos_of_node
				for (auto i = p_in_pos; i < d_in_pos; ++i)
				{
					++_pos_of_node[sol->NodeAt(i)];
				}
				for (auto i = d_in_pos; i <= sol->GetNbNodes(); ++i)	//sol->GetNbnodes = N - 2;
				{
					_pos_of_node[sol->NodeAt(i)] += 2;
				}

				_pos_of_node[x_p] = p_in_pos;
				_pos_of_node[x_d] = d_in_pos + 1;


				double in_cost = current_record.InCost;
				sol->InsertNodeAtPosition(x_d, d_in_pos);
				sol->InsertNodeAtPosition(x_p, p_in_pos);
				ForbidRmove_Range(sol, p_in_pos - 1, d_in_pos + 2);


				sol->UpDataLoadStatus(p_in_pos, d_in_pos + 1);	//!!!
				sol->ChangeCost(in_cost);

				this->mv_job_vec.push_back(x_p);
				
				continue;	
			}

			if (!current_couple.record_vec.empty())
			{
				Candidatequeue.emplace(current_couple);
				//std::cout << "re insert to que :: " <<  current_couple.request_id << std::endl;
			}

			//取消掉之前的remove操作
			sol->InsertNodeAtPosition(x_p, Rx_p_pos);
			sol->InsertNodeAtPosition(x_d, Rx_d_pos);

			sol->UpDataLoadStatus(Rx_p_pos, Rx_d_pos);

			sol->ChangeCost(rm_cost);


			for (auto i = Rx_p_pos; i < Rx_d_pos; ++i)
			{
				++_pos_of_node[sol->NodeAt(i)];
			}
			for (auto i = Rx_d_pos + 1; i <= 2 * N; ++i)
			{
				_pos_of_node[sol->NodeAt(i)] += 2;
			}

			_pos_of_node[x_p] = Rx_p_pos;
			_pos_of_node[x_d] = Rx_d_pos;
			
		}
	}

	//std::cout << "reloacte jobs : " << n << std::endl;
	return;
}


void RelocateMethod::Optimize(Solution*& sol, TabuOperatorList* tbls)
{

	int N = sol->GetNbJobs();
	if (_pos_of_node != NULL) { delete[] _pos_of_node; _pos_of_node = NULL; }
	_pos_of_node = new int[2 * N + 2];
	_pos_of_node[0] = -1;

	for (auto i = 0u; i <= sol->GetNbNodes(); ++i)
	{
		_pos_of_node[sol->NodeAt(i)] = i;
	}


	//===================调试===================

	this->GenerateCandidateQue(sol, tbls);


	while (!this->Candidatequeue.empty())
	{
		RelocateMultiCouple(sol);

		this->GenerateCandidateQue(sol, tbls);
	}
	



	delete[] _pos_of_node;	    _pos_of_node = NULL;
	delete[] _Rm_Flag_of_node;	_Rm_Flag_of_node = NULL;
	delete[] _In_Flag_of_node;	_In_Flag_of_node = NULL;

	this->mv_job_vec.clear();
	return;
}


