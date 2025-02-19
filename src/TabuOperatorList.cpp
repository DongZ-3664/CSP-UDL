#include "TabuOperatorList.h"


//=======================================definition====================================
TabuOperatorList::TabuOperatorList(int cap, int tenure, double forget_rate)
{
	this->tabu_list.clear();
	this->exist_flag.clear();

	this->capatity = cap;
	this->tabu_tenure = tenure;
	this->forget_length = capatity * forget_rate;
}


TabuOperatorList::TabuOperatorList()
{

}

TabuOperatorList::~TabuOperatorList()
{
	
}



bool TabuOperatorList::Find(int tb_job)
{
	if (this->tabu_list.empty()) return false;

	return this->exist_flag[tb_job];

}


void TabuOperatorList::Remember(int tb_job)
{

	if (this->tabu_list.size() == this->capatity)
	{
		this->DeletePart(0, forget_length);
	}


	this->tabu_list.push_back({ tb_job,tabu_tenure });

	this->exist_flag[tb_job] = true;

	return;
}


void TabuOperatorList::DeleteEarliest()
{
	if (this->tabu_list.empty())
	{
		std::cerr << "Fail to delete, current Tabu list is empty !" << std::endl;
		return;
	}

	//this->exist_flag.erase(tabu_list.front());

	this->exist_flag.erase(tabu_list[0][0]);

	this->tabu_list.erase(tabu_list.begin());
}


void TabuOperatorList::DeletePart(int s_pos, int e_pos)
{
	if (this->tabu_list.size() <= e_pos || e_pos < s_pos)
	{
		throw IllegalParameters("Check input parameters !");
	}

	for (int i = s_pos; i <= e_pos; ++i)	// [s_pos, e_pos];
	{

		this->exist_flag.erase(tabu_list[i][0]);

	}

	this->tabu_list.erase(this->tabu_list.begin() + s_pos, this->tabu_list.begin() + e_pos + 1);	// [s_pos, e_pos];
}



void TabuOperatorList::ClearMemory()
{
	this->tabu_list.clear();
	this->exist_flag.clear();
}



int TabuOperatorList::GetSize()
{
	return this->tabu_list.size();
}



int TabuOperatorList::GetTenure()
{
	return this->tabu_tenure;
}


void TabuOperatorList::PrintTabuList()
{

	for (int i = 0; i < tabu_list.size(); ++i)
	{
		std::cout << tabu_list[i][0] << ", ";


	}
	std::cout << std::endl;
}




//添加的方法
void TabuOperatorList::Updata()
{
	
	int i = 0;
	while (i < tabu_list.size() && tabu_list[i][1] <= 1)
	{
		++i;
	}

	if (1 <= i)
	{
		this->DeletePart(0, i - 1);
	}

	for (auto& s : tabu_list)
	{
		--s[1];
	}
}
