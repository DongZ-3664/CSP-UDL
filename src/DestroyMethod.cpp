#include "DestroyMethod.h"



DestroyMethod::DestroyMethod(std::string name, int num)
{
	this->OperatorName = name;
	this->num_rm_jobs = num;
}

DestroyMethod::~DestroyMethod()
{

}


int DestroyMethod::RandomAPos(int num_job, int rand_degree)
{
	int rand_int = RandNum(0, 99);

	double rand_fraction = (double)rand_int / 100;
	int rand_pos = floor(pow(rand_fraction, rand_degree) * num_job);

	return rand_pos;
}


void MoveMaxtoLeft(std::vector<std::pair<int, double>>& vec, int left)	
{
	int max = left;
	for (int i = left; i < vec.size(); i++)
	{
		if (vec[i].second > vec[max].second)
		{
			max = i;
		}
	}

	std::swap(vec[left], vec[max]);
}


void DestroyMethod::SetScore(double s1, double s2, double s3)
{
	this->random_removal_score = s1;
	this->nearest_removal_score = s2;
	this->worst_removal_score = s3;
}


double DestroyMethod::GetScore(int method_type)
{

	if (method_type == 1)
	{
		return random_removal_score;
	}

	if (method_type == 2)
	{
		return nearest_removal_score;
	}

	return worst_removal_score;
}

void DestroyMethod::UpdataWeights(int method_type, double delta)
{
	if (method_type == 1)
	{
		random_removal_score *= (1.0 - _WEIGHTS_SENSITIVITY);
		random_removal_score += _WEIGHTS_SENSITIVITY * delta;
		return;
	}

	if (method_type == 2)
	{
		nearest_removal_score *= (1.0 - _WEIGHTS_SENSITIVITY);
		nearest_removal_score += _WEIGHTS_SENSITIVITY * delta;
		return;
	}

	worst_removal_score *= (1.0 - _WEIGHTS_SENSITIVITY);
	worst_removal_score += _WEIGHTS_SENSITIVITY * delta;
	return;
}

int DestroyMethod::ChooseARemovalMethod()
{
	double max_s = random_removal_score + nearest_removal_score + worst_removal_score;

	double rd_s = RandDouble(0.0, max_s);

	if (rd_s <= random_removal_score)
	{
		return 1;
	}

	if (rd_s <= random_removal_score + nearest_removal_score)
	{
		return 2;
	}

	return 3;
}

void DestroyMethod::NearestRemoval(Solution*& ISol, TabuOperatorList* tbls, int rand_degree)
{
	std::vector<int> job_set = ISol->GetInstance()->JobSet;

	int N = ISol->GetNbJobs();
	int rm_num = 1;
	rm_job_vec.clear();

	int cur_job = RandNum(1, N);
	job_set.erase(job_set.begin() + cur_job - 1);

	//remove
	int rm_p_pos = ISol->IndexOf(cur_job);
	int rm_d_pos = ISol->IndexOf(cur_job + N);

	rm_job_vec.push_back(cur_job);

	ISol->RemoveJob(rm_job_vec[0], rm_p_pos, rm_d_pos);

	std::vector<std::pair<int, double>> job_dis_vec;
	double** D = ISol->GetInstance()->Distance;
	int m = this->num_rm_jobs;

	while (--m)
	{
		++rm_num;
		cur_job = rm_job_vec.back();

		for (auto job : job_set)
		{
			double job_dis = D[cur_job][job] + D[cur_job + N][job + N];
			job_dis_vec.push_back(std::make_pair(job, job_dis));
		}


		std::sort(job_dis_vec.begin(), job_dis_vec.end(), cmp5);

		int rand_rm_pos = this->RandomAPos(job_set.size(), rand_degree);
		//rand_rm_pos = 0;
		int rm_job = job_dis_vec[rand_rm_pos].first;


		if (tbls->Find(rm_job))
		{
			continue;
		}

		std::vector<int>::iterator pos_in_js = std::find(job_set.begin(), job_set.end(), rm_job);

		job_set.erase(pos_in_js);
		job_dis_vec.clear();

		//remove
		int rm_p_pos = ISol->IndexOf(rm_job);
		int rm_d_pos = ISol->IndexOf(rm_job + N);
		double rm_cost = ISol->GainRemovalSub(rm_job, rm_p_pos, rm_d_pos);

		rm_job_vec.push_back(rm_job);

		ISol->RemoveLoad(rm_p_pos, rm_d_pos);
		ISol->RemoveNodeAtPosition(rm_d_pos);
		ISol->RemoveNodeAtPosition(rm_p_pos);
		ISol->ChangeCost(-rm_cost);

	}

	return;
}

void DestroyMethod::RandomRemoval(Solution*& ISol, TabuOperatorList* tbls, int rand_degree)
{
	std::vector<int> job_set = ISol->GetInstance()->JobSet;
	int N = ISol->GetNbJobs();
	rm_job_vec.clear();

	int rand_rm_seed = rand();

	//int rand_rm_seed = 5;
	std::srand(rand_rm_seed);
	std::random_shuffle(job_set.begin(), job_set.end());

	int m = this->num_rm_jobs;
	while (m--)
	{
		int cur_rm_job = job_set.back();
		job_set.pop_back();

		if (tbls->Find(cur_rm_job))
		{
			continue;
		}

		double cur_rm_sub = ISol->GainRemovalSub(cur_rm_job);
		this->rm_job_vec.push_back(cur_rm_job);

		//remove
		int rm_p_pos = ISol->IndexOf(cur_rm_job);
		int rm_d_pos = ISol->IndexOf(cur_rm_job + N);

		ISol->RemoveJob(cur_rm_job, rm_p_pos, rm_d_pos);

	}
	return;

}

void DestroyMethod::PrintRmJobs()
{
	std::cerr << "The set of removed jobs : { ";
	for (auto s : this->rm_job_vec)
	{
		std::cerr << s << ",";
	}
	std::cerr << " }" << std::endl;
}

std::vector<int> DestroyMethod::GetMoveJobs()
{
	return this->rm_job_vec;
}

void DestroyMethod::WorstRemoval(Solution*& ISol, TabuOperatorList* tbls, int rand_degree)
{
	std::vector<int> job_set = ISol->GetInstance()->JobSet;
	int N = ISol->GetNbJobs();
	rm_job_vec.clear();

	int rm_num = 1;

	std::vector<std::pair<int, double>> job_cost_vec;

	int m = this->num_rm_jobs;
	while (m--)
	{

		for (auto job : job_set)
		{
			double rm_sub = ISol->GainRemovalSub(job);
			job_cost_vec.push_back(std::make_pair(job, rm_sub));
		}

		int rand_rm_pos = this->RandomAPos(job_set.size(), rand_degree);
		if (rand_rm_pos <= 4)	//5or4
		{
			int l = 0;
			while (l <= rand_rm_pos)
			{
				MoveMaxtoLeft(job_cost_vec, l);
				++l;
			}
		}
		else
		{
			std::sort(job_cost_vec.begin(), job_cost_vec.end(), cmp4);
		}


		int rm_job = job_cost_vec[rand_rm_pos].first;


		if (tbls->Find(rm_job))
		{
			continue;
		}


		std::vector<int>::iterator pos_in_js = std::find(job_set.begin(), job_set.end(), rm_job);

		job_set.erase(pos_in_js);
		this->rm_job_vec.push_back(rm_job);
		job_cost_vec.clear();


		//remove
		ISol->RemoveJob(rm_job);
	}

	return;
}












