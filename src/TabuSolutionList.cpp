#include "TabuSolutionList.h"


//=======================================definition====================================
TabuSolutionList::TabuSolutionList(int cap, int tenure, double forget_rate)
{
	this->tabu_list.clear();
	this->exist_flag.clear();

	this->capatity = cap;
	this->tabu_tenure = tenure;
	this->forget_length = capatity * forget_rate;
}


TabuSolutionList::TabuSolutionList()
{

}

TabuSolutionList::~TabuSolutionList()
{
	
}



bool TabuSolutionList::Find(Solution* sol)
{
	if (this->tabu_list.empty()) return false;


	if (this->exist_flag[sol->GetTotalCosts()])
	{

		TabuObject object(sol->GetPath(), sol->GetTotalCosts());


		for (auto i = 0u; i < tabu_list.size(); ++i)
		{
			if ((*tabu_list[i]) == object) { return true; }			

		}
	}

	return false;
}


void TabuSolutionList::Remember(Solution* sol)
{

	TabuObject* object = new TabuObject(sol->GetPath(), sol->GetTotalCosts());

	if (this->tabu_list.size() == this->capatity)
	{
		this->DeletePart(0, forget_length);
	}

	this->tabu_list.push_back(object);
	this->exist_flag[object->total_cost] = true;

	return;
}


void TabuSolutionList::DeleteEarliest()
{
	if (this->tabu_list.empty())
	{
		std::cerr << "Fail to delete, current Tabu list is empty !" << std::endl;
		return;
	}
	
	this->exist_flag.erase(tabu_list.front()->total_cost);
	this->tabu_list.front()->~TabuObject();
	this->tabu_list.erase(tabu_list.begin());
}


void TabuSolutionList::DeletePart(int s_pos, int e_pos)
{
	if (this->tabu_list.size() <= e_pos || e_pos < s_pos)
	{
		throw IllegalParameters("Check input parameters !");
	}

	for (int i = s_pos; i <= e_pos; ++i)	// [s_pos, e_pos];
	{
		this->exist_flag.erase(tabu_list[i]->total_cost);
		this->tabu_list[i]->~TabuObject();
	}

	this->tabu_list.erase(this->tabu_list.begin() + s_pos, this->tabu_list.begin() + e_pos + 1);	// [s_pos, e_pos];
}



void TabuSolutionList::ClearMemory()
{
	this->tabu_list.clear();
	this->exist_flag.clear();
}

int TabuSolutionList::GetSize()
{
	return this->tabu_list.size();
}

int TabuSolutionList::GetTenure()
{
	return this->tabu_tenure;
}


void TabuSolutionList::PrintTabuList()
{
	for (int i = 0; i < tabu_list.size(); ++i)
	{
		std::cout << tabu_list[i] << ", ";
	}
	std::cout << std::endl;
}




//添加的方法
void TabuSolutionList::Updata()
{
	
	if (0 < tabu_list.size() && tabu_list[0]->rest == 1)
	{
		this->DeleteEarliest();
	}

	for (auto& s : tabu_list)
	{
		--s->rest;
	}

}




