#include "Solution.h"
#include "RelocateMethod.h"
#include "Compare.h"

// Member function of class Solution
Solution::Solution(std::vector<int>& sol_vec, Instance* ins):NbJobs(ins->InsNbJobs)
{
	this->XPath = sol_vec;

	CurrentInstance = ins;	//浅copy

	//NbJobs = ins->InsNbJobs;
	NbNodes = sol_vec.size();

	Load.resize(NbNodes);
	UpDataLoadStatus(0, NbNodes - 1);
	TotalCosts = CalculateTotalCosts();
}


Solution::Solution(Solution* sol) 
{
	//this->CurrentInstance = new Instance(sol->GetInstance());
	this->CurrentInstance = sol->CurrentInstance;	//使用被复制的solution对象的 currentinstance 成员地址；

													
	this->NbJobs = sol->NbJobs;
	this->NbNodes = sol->NbNodes;
	this->TotalCosts = sol->TotalCosts;

	this->XPath = sol->XPath;
	this->Load = sol->Load;

}


Solution::Solution() :NbJobs(0), NbNodes(0)
{

}

Solution::~Solution()
{

}

void Solution::PrintSolutionInformation()
{
	std::cerr << "+--------";
	std::cerr << " Information of the best solution : ";
	std::cerr << "---------+" << std::endl;
	
	
	//Output the path of current solution;
	std::cerr << "| Current Path : " <<std::setw(38) <<std::right <<"|" << std::endl << "| ";
	for (int i = 0; i < XPath.size(); ++i)
	{
		std::cerr << std::setw(4) << std::right << XPath[i] << ",";
		if (i % 10 == 9)
		{
			std::cerr << "  |" << std::endl << "| ";
		}
	}
	

	/*
	//Output load status after every node, the number imply that the number of loads;
	std::cerr << std::endl;
	std::cerr << "Load status : " << std::endl;
	for (auto s : this->Load)
	{
		std::cerr << s << "  ";
	}
	std::cerr << std::endl;


	//Output operations, the number 1 refer to lift up a job, -1 refer to drop off a job;
	std::cerr << "Operations : " << std::endl;
	for (auto i = 0; i < this->NbNodes; ++i)
	{
		std::cerr << this->Operation(i) << "  ";
	}
	std::cerr << std::endl;
	*/


	std::cerr << "obj value of the best solution : " << std::setw(16) << std::right
			  << this->CalculateTotalCosts() << "   |" << std::endl;
	

	std::cerr << "+-----------------------------------------------------+" << std::endl;
}



void Solution::UpDataLoadStatus(int start_pos, int end_pos)
{
	if (start_pos < 0 || end_pos > NbNodes)
	{
		throw IllegalParameters("Updata Load out of range !");
	}

	if (start_pos == 0)
	{
		this->Load[start_pos] = 1;
		++start_pos;
	}

	if (end_pos == NbNodes)
	{
		--end_pos;
	}

	for (auto i = start_pos; i <= end_pos; ++i)
	{
		Load[i] = Load[i - 1] + Operation(i);
	}
	return;
}


void Solution::RemoveLoad(int start_pos, int end_pos)
{
	if (start_pos < 0 || end_pos > NbNodes)
	{
		throw IllegalParameters("Remove Load index out of range !");
	}

	for (int i = start_pos; i <= end_pos; i++)
	{
		--this->Load[i];
	}
}

int Solution::CurrentMaxLoad()
{
	int max_load = Load[0];
	for (auto i = 0u; i < NbNodes; ++i)
	{
		if (Load[i] >= max_load)
		{
			max_load = Load[i];
		}
	}
	return max_load;
}


int Solution::LoadAtPos(int pos)
{
	if (pos < 0 || pos > NbNodes)
	{
		throw IllegalParameters("This position out of range !");
	}
	return this->Load[pos];
}

int Solution::NodeAt(int pos)
{
	if (pos == this->NbNodes)
	{
		return 2 * NbJobs + 1;
	}
	return this->XPath[pos];
}

int Solution::GetDeliveryNode(int pickup_node_id)
{
	if (pickup_node_id <= NbJobs)
		return (pickup_node_id + this->NbJobs);
	else
		throw IllegalParameters("This is not a pick up node !");
}

double Solution::CalculateTotalCosts()
{
	double cost = 0;
	for (auto i = 0u; i <= this->NbNodes - 2; ++i)		// i + 1 < NbNodes
	{
		cost += Speed(this->Load[i]) * \
			this->CurrentInstance->Distance[XPath[i]][XPath[i + 1]];
	}
	this->TotalCosts = cost;

	return cost;
}

double Solution::GetTotalCosts()
{
	return this->TotalCosts;
}


double Solution::IntervalCosts(int start_pos, int end_pos)
{
	if (start_pos < 0)
	{
		start_pos = 0;
	}
	
	
	if(end_pos == NbNodes)
	{
		end_pos = NbNodes - 1;
	}

	double cost = 0;
	for (int i = start_pos; i < end_pos; ++i)		
	{
		cost += Speed(this->Load[i]) * \
			this->CurrentInstance->Distance[XPath[i]][XPath[i + 1]];
	}
	return cost;
}

bool Solution::IsFeasible()
{
	if (this->XPath.size() != 2 * NbJobs)
	{
		std::cout << "solution is infeasible, cause: wrong number of nodes !" << std::endl;
		return false;
	}
	bool a = this->CurrentMaxLoad() <= CurrentInstance->Capacity;
	bool b = this->CanBeReversed();

	return a && b;
}

std::vector<int> Solution::GetDInsertedSuc(int pos)
{
	std::vector<int> D_node_inserted_suc;
	if (pos == this->XPath.size())
	{
		D_node_inserted_suc.push_back(2 * NbJobs + 1);
		return D_node_inserted_suc;
	}
	bool b = XPath[pos] >= NbJobs + 1 && Load[pos] + 1 >= this->GetInstance()->Capacity;
	if (b)
	{
		return D_node_inserted_suc;
	}

	D_node_inserted_suc.push_back(XPath[pos]);
	for (int i = pos; i < this->XPath.size() && Load[i] != Load[pos] - 2; ++i)
	{
		if (this->Load[i] == this->Load[pos] - 1)
		{
			if (i == XPath.size() - 1)
				D_node_inserted_suc.push_back(2 * NbJobs + 1);
			else
				D_node_inserted_suc.push_back(XPath[i + 1]);
		}
		if (Load[i] >= this->GetInstance()->Capacity)
			break;
	}

	return D_node_inserted_suc;

}

std::vector<int> Solution::GetDInsertedPos(int pos)
{
	std::vector<int> D_node_inserted_pos;
	if (pos == this->XPath.size())
	{
		D_node_inserted_pos.push_back(pos);
		return D_node_inserted_pos;
	}
	bool b = XPath[pos] >= NbJobs + 1 && Load[pos] + 1 >= this->GetInstance()->Capacity;
	if (b)
	{
		return D_node_inserted_pos;
	}

	D_node_inserted_pos.push_back(pos);
	for (int i = pos; i < this->XPath.size() && Load[i] != Load[pos] - 2; ++i)
	{
		if (this->Load[i] == this->Load[pos] - 1)
		{
			D_node_inserted_pos.push_back(i + 1);
		}
		if (Load[i] >= this->GetInstance()->Capacity)
			break;
	}

	return D_node_inserted_pos;


}

std::vector<int> Solution::ConvertToIndex(std::vector<int>& a_suc_vec)
{
	std::vector<int> index_vec;

	for (auto i = 0u; i < a_suc_vec.size(); ++i)
	{
		if (a_suc_vec[i] == 2 * NbJobs + 1)
		{
			index_vec.push_back(this->NbNodes);
		}
		else
		{
			index_vec.push_back(this->IndexOf(a_suc_vec[i]));
		}
	}

	if (index_vec.size() != a_suc_vec.size())
	{
		throw IllegalParameters("Exist node of not found !");
	}


	return index_vec;
	/*the return value is a vector whcih element imply that index of each successor*/
}



double	Solution::GainInertionAdd(int job_id, int p_in_pos, int d_in_pos)
{

	double cost_delta_l = 0;
	double cost_delta_r = 0;
	double cost_delta_m = 0;
	double delta = 0;
	int load_p = 0;		//load_d = load_p - 1;
	int N = this->NbJobs;
	double** D = this->CurrentInstance->Distance;

	if (p_in_pos == d_in_pos)
	{
		if (p_in_pos == 0)
		{
			load_p = 1;
			delta = Speed(load_p) * D[job_id][job_id + N] + \
				Speed(load_p - 1) * D[job_id + N][this->XPath[0]];
		}
		else if (p_in_pos == this->NbNodes)
		{
			load_p = 1;
			delta = Speed(0) * D[this->XPath.back()][job_id] + \
				Speed(load_p) * D[job_id][job_id + N];
		}
		else
		{
			load_p = this->Load[p_in_pos - 1] + 1;
			delta = Speed(load_p - 1) * D[this->XPath[p_in_pos - 1]][job_id] + \
				Speed(load_p) * D[job_id][job_id + N] + \
				Speed(load_p - 1) * D[job_id + N][this->XPath[p_in_pos]] - \
				Speed(load_p - 1) * D[this->XPath[p_in_pos - 1]][this->XPath[p_in_pos]];
		}
	}
	else	//p_in_pos != d_in_pos
	{
		if (p_in_pos == 0)
		{
			load_p = 1;
			cost_delta_l = Speed(load_p) * D[job_id][this->XPath[0]];
		}
		else
		{
			load_p = this->Load[p_in_pos - 1] + 1;
			cost_delta_l = Speed(load_p - 1) * D[this->XPath[p_in_pos - 1]][job_id] + \
				Speed(load_p) * D[job_id][this->XPath[p_in_pos]] - \
				Speed(load_p - 1) * D[this->XPath[p_in_pos - 1]][this->XPath[p_in_pos]];
		}


		if (d_in_pos == this->NbNodes)
		{
			cost_delta_r = Speed(load_p) * D[this->XPath.back()][job_id + N];	 
		}
		else
		{
			cost_delta_r = Speed(load_p) * D[this->XPath[d_in_pos - 1]][job_id + N] +
				Speed(load_p - 1) * D[job_id + N][this->XPath[d_in_pos]] - \
				Speed(load_p - 1) * D[this->XPath[d_in_pos - 1]][this->XPath[d_in_pos]];
		}

		for (auto i = p_in_pos; i < d_in_pos - 1; i++)
		{
			cost_delta_m += (Speed(this->Load[i] + 1) - Speed(this->Load[i])) * \
				D[this->XPath[i]][this->XPath[i + 1]];
		}

		delta = cost_delta_l + cost_delta_m + cost_delta_r;
	}



	return delta;
}

double	Solution::GainRemovalSub(int job_id, int rx_p_pos, int rx_d_pos)
{

	if (this->NbNodes <= 2)
	{
		std::cerr << "The current solution consists of only one job (two nodes) !" << std::endl;
		return this->TotalCosts;
	}

	double cost_delta_l = 0;
	double cost_delta_r = 0;
	double cost_delta_m = 0;
	double delta = 0;
	int load_p = 0;		//load_d = load_p - 1;
	int N = this->NbJobs;

	if (rx_p_pos == -1 || rx_d_pos == -1)
	{
		rx_p_pos = this->IndexOf(job_id);
		rx_d_pos = this->IndexOf(job_id + N);
	}

	double**& D = this->CurrentInstance->Distance;
	if (rx_d_pos == rx_p_pos + 1)	//Job (job_id) is droped off after lifted up it immediately
	{
		if (rx_p_pos == 0)
		{
			load_p = 1;
			delta = Speed(load_p) * D[job_id][job_id + N] + \
				Speed(load_p - 1) * D[job_id + N][this->XPath[2]];
		}
		else if (rx_p_pos == this->NbNodes - 2)
		{
			load_p = 1;
			delta = Speed(0) * D[this->XPath[rx_p_pos - 1]][job_id] + \
				Speed(load_p) * D[job_id][job_id + N];
		}
		else
		{
			load_p = this->Load[rx_p_pos];
			delta = Speed(load_p - 1) * D[this->XPath[rx_p_pos - 1]][job_id] + \
				Speed(load_p) * D[job_id][job_id + N] + \
				Speed(load_p - 1) * D[job_id + N][this->XPath[rx_d_pos + 1]] - \
				Speed(load_p - 1) * D[this->XPath[rx_p_pos - 1]][this->XPath[rx_d_pos + 1]];
		}
	}
	else	//rx_p_pos + 1 < rx_d_pos
	{
		if (rx_p_pos == 0)
		{
			load_p = 1;
			cost_delta_l = Speed(load_p) * D[job_id][this->XPath[1]];
		}
		else
		{
			load_p = this->Load[rx_p_pos];
			cost_delta_l = Speed(load_p - 1) * D[this->XPath[rx_p_pos - 1]][job_id] + \
				Speed(load_p) * D[job_id][this->XPath[rx_p_pos + 1]] - \
				Speed(load_p - 1) * D[this->XPath[rx_p_pos - 1]][this->XPath[rx_p_pos + 1]];
		}


		if (rx_d_pos == this->NbNodes - 1)
		{
			cost_delta_r = Speed(load_p) * D[this->XPath[rx_d_pos - 1]][job_id + N];
		}
		else
		{
			cost_delta_r = Speed(load_p) * D[this->XPath[rx_d_pos - 1]][job_id + N] +
				Speed(load_p - 1) * D[job_id + N][this->XPath[rx_d_pos + 1]] - \
				Speed(load_p - 1) * D[this->XPath[rx_d_pos - 1]][this->XPath[rx_d_pos + 1]];
		}

		for (auto i = rx_p_pos + 1; i < rx_d_pos - 1; i++)
		{
			cost_delta_m += (Speed(this->Load[i]) - Speed(this->Load[i] - 1)) * \
				D[this->XPath[i]][this->XPath[i + 1]];
		}

		delta = cost_delta_l + cost_delta_m + cost_delta_r;
	}




	return delta;
}

Solution* Solution::RemoveJob(int job_id, int rm_p_pos, int rm_d_pos)
{

	if (rm_p_pos == -1 || rm_d_pos == -1)
	{
		rm_p_pos = this->IndexOf(job_id);
		rm_d_pos = this->IndexOf(job_id + this->NbJobs);
	}

	double sub = GainRemovalSub(job_id, rm_p_pos, rm_d_pos);

	this->RemoveLoad(rm_p_pos, rm_d_pos);
	this->RemoveNodeAtPosition(rm_d_pos);
	this->RemoveNodeAtPosition(rm_p_pos);
	this->TotalCosts -= sub;


	return this;
}



Solution* Solution::RemoveNodeAtPosition(int pos)
{
	if (pos < 0 || pos >= NbNodes)
	{
		throw IllegalParameters("removed position out of range !");
	}

	else
	{
		this->XPath.erase(this->XPath.begin() + pos);
		this->Load.erase(this->Load.begin() + pos);

		--NbNodes;
		return this;
	}
}

Solution* Solution::InsertNodeAtPosition(int node, int pos)	// Inserts a point to the pos in the path
{
	if (pos < 0 || pos >= this->XPath.size() + 1)
	{
		throw IllegalParameters("The position of the inserted node is out of range !");
	}
	this->XPath.insert(this->XPath.begin() + pos, node);

	this->Load.insert(this->Load.begin() + pos, 0);
	++NbNodes;
	return this;
}

Instance* Solution::GetInstance()
{
	return this->CurrentInstance;
}

void Solution::ChangeNode(int job_id, int pos)
{
	this->XPath[pos] = job_id;
}


bool Solution::IsPickUpNode(int pos)
{
	return(this->XPath[pos] <= this->NbJobs);
}


int Solution::IndexOf(int node)					//View index of a node in the path
{
	int Index = -1;

	std::vector<int>::iterator it = std::find(XPath.begin(), XPath.end(), node);
	Index = (int)(it - XPath.begin());

	if (Index == -1)
	{
		throw IllegalParameters("Not find this node !");
	}

	return Index;
}

bool Solution::CanBeReversed()
{
	if (this->XPath.front() <= NbJobs &&
		this->XPath.back() >= NbJobs + 1)		// ↑......↓
	{
		std::stack<int> CheckReversiable;
		std::vector<int>::iterator it = this->XPath.begin();

		while (it != this->XPath.end())
		{
			if (CheckReversiable.empty())
			{
				CheckReversiable.push(*it);

			}
			else if (!CheckReversiable.empty() && *it == CheckReversiable.top() + NbJobs)
			{
				CheckReversiable.pop();
			}
			else
			{
				CheckReversiable.push(*it);
			}
			it++;
		}

		if (CheckReversiable.empty())
			return true;
		else
			return false;
	}
	else
	{
		return false;
	}
}


const int Solution::GetNbJobs()
{
	return this->NbJobs;
}

int Solution::GetNbNodes()
{
	return this->NbNodes;
}

void Solution::ChangeCost(double delta)
{
	this->TotalCosts += delta;
}

std::vector<int> Solution::GetPath()
{
	return this->XPath;
}

std::vector<int> Solution::GetRelativeJob(int rx_p_pos, int rx_d_pos)
{
	if (this->XPath[rx_d_pos] != this->XPath[rx_p_pos] + NbJobs)
	{
		throw IllegalParameters("Input parameters error !");
	}


	while (true)
	{
		if (this->XPath[rx_p_pos] <= this->NbJobs && this->Load[rx_p_pos] == 1)
			break;
		else { --rx_p_pos; }
	}
	while (true)
	{
		if (this->XPath[rx_d_pos] >= this->NbJobs + 1 && this->Load[rx_d_pos] == 0)
			break;
		else { ++rx_d_pos; }

	}

	std::vector<int> res;

	if (rx_p_pos > 0)
	{
		res.push_back(this->XPath[rx_p_pos - 1] - this->NbJobs);
	}

	for (int i = rx_p_pos; i < rx_d_pos; ++i)
	{
		if (this->XPath[i] <= this->NbJobs)
		{
			res.push_back(this->XPath[i]);
		}
		else
		{
			continue;
		}
	}

	if (rx_d_pos < this->NbNodes - 1) 
	{
		res.push_back(this->XPath[rx_d_pos + 1]);
	}

	return res;
}

std::vector<int> Solution::GetRelativeJobTEST(int rx_p_pos, int rx_d_pos)
{


	while (true)
	{
		if (this->XPath[rx_p_pos] <= this->NbJobs && this->Load[rx_p_pos] == 1)
			break;
		else { --rx_p_pos; }
	}
	while (true)
	{
		if (this->XPath[rx_d_pos] >= this->NbJobs + 1 && this->Load[rx_d_pos] == 0)
			break;
		else { ++rx_d_pos; }

	}

	std::vector<int> res;

	if (rx_p_pos > 0)
	{
		res.push_back(this->XPath[rx_p_pos - 1] - this->NbJobs);
	}

	for (int i = rx_p_pos; i < rx_d_pos; ++i)
	{
		if (this->XPath[i] <= this->NbJobs)
		{
			res.push_back(this->XPath[i]);
		}
		else
		{
			continue;
		}
	}

	if (rx_d_pos < this->NbNodes - 1)
	{
		res.push_back(this->XPath[rx_d_pos + 1]);
	}

	return res;
}



void Solution::ChangeJob(int job_id, int p_pos, int d_pos)
{
	double no_ex_cost = IntervalCosts(p_pos - 1, d_pos + 1);

	this->ChangeNode(job_id, p_pos);
	this->ChangeNode(job_id + NbJobs, d_pos);

	double ex_cost = IntervalCosts(p_pos - 1, d_pos + 1);
	this->TotalCosts -= no_ex_cost;
	this->TotalCosts += ex_cost;
}



//===========test module========
int Solution::Operation(int pos)
{
	if (this->XPath[pos] <= NbJobs)
	{
		return 1;
	}
	return -1;
}


bool Solution::operator==(Solution& sol)
{
	if (std::fabs(this->TotalCosts - sol.TotalCosts) < __EPSILON)
	{
		for (auto i = 0u; i < this->NbNodes; i++)
		{
			if (this->XPath[i] != sol.XPath[i])
				return false;
		}
		return true;
	}

	return false;
}
