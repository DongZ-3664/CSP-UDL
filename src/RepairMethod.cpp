#include "RepairMethod.h"



RepairMethod::RepairMethod(std::string name)
{
	this->OperatorName = name;
}

RepairMethod::~RepairMethod()
{
	
}

void RepairMethod::SetScore(double s1, double s2, double s3)
{
	this->rand_cheapest_insertion_score = s1;
	this->regret_insertion_score = s2;
	this->cheapest_insertion_score = s3;
}


double RepairMethod::GetScore(int method_type)
{
	if (method_type == 1)
	{
		return rand_cheapest_insertion_score;
	}

	if (method_type == 2)
	{
		return regret_insertion_score;
	}

	return cheapest_insertion_score;
}


void RepairMethod::UpdataWeights(int method_type, double delta)
{
	if (method_type == 1)
	{
		rand_cheapest_insertion_score *= (1.0 - _WEIGHTS_SENSITIVITY);
		rand_cheapest_insertion_score += _WEIGHTS_SENSITIVITY * delta;
		return;
	}

	if (method_type == 2)
	{
		regret_insertion_score *= (1.0 - _WEIGHTS_SENSITIVITY);
		regret_insertion_score += _WEIGHTS_SENSITIVITY * delta;
		return;
	}

	cheapest_insertion_score *= (1.0 - _WEIGHTS_SENSITIVITY);
	cheapest_insertion_score += _WEIGHTS_SENSITIVITY * delta;
	return;

}


int RepairMethod::ChooseAInsertionMethod()
{
	double max_s = rand_cheapest_insertion_score + regret_insertion_score + cheapest_insertion_score;
	double rd_s = RandDouble(0, max_s);
	if (rd_s <= rand_cheapest_insertion_score)
	{
		return 1;
	}

	if (rd_s <= rand_cheapest_insertion_score + regret_insertion_score)
	{
		return 2;
	}

	return 3;
}


void RepairMethod::RandCheapestInsertion(Solution*& ISol, DestroyMethod* DesM)
{
	int N = ISol->GetNbJobs();
	int M = 999999;

	while (!DesM->rm_job_vec.empty())
	{
		int best_Ix_p_pos;
		int best_Ix_d_pos;
		int next_Ix = DesM->rm_job_vec.back();
		DesM->rm_job_vec.pop_back();

		double min_delta = M;

		for (int Ix_p_pos = 0; Ix_p_pos <= ISol->GetNbNodes(); ++Ix_p_pos)
		{
			std::vector<int> Ix_d_pos_vec = ISol->GetDInsertedPos(Ix_p_pos);

			for (auto Ix_d_pos : Ix_d_pos_vec)
			{
				double cur_delta = ISol->GainInertionAdd(next_Ix, Ix_p_pos, Ix_d_pos);

				if (cur_delta < min_delta)
				{
					min_delta = cur_delta;
					best_Ix_p_pos = Ix_p_pos;
					best_Ix_d_pos = Ix_d_pos;
				}
			}
		}

		//Insert the cheapest job into isol;
		ISol->InsertNodeAtPosition(next_Ix + N, best_Ix_d_pos);
		ISol->InsertNodeAtPosition(next_Ix, best_Ix_p_pos);
		ISol->UpDataLoadStatus(best_Ix_p_pos, best_Ix_d_pos + 1);	// + 1 !!!

		//std::cout << "insert jobs : " << next_Ix.first << "into :"
		//	<< best_Ix_p_pos << ", " << best_Ix_d_pos << "delta :" << min_delta
		//	<< std::endl;
		ISol->ChangeCost(min_delta);
	}

	return;
}

void MoveMintoLeft(std::vector<Record>& vec, int left)	//[0, right] left <- min
{
	int min = left;
	for (int i = left; i < vec.size(); i++)
	{
		if (vec[i].InCost <= vec[min].InCost)
		{
			min = i;
		}
	}

	std::swap(vec[left], vec[min]);
}

double RepairMethod::GainRegretValue(Solution* ISol, int in_job_id)
{
	int k = k_step;
	this->record_vec.clear();

	for (auto Ix_p_pos = 0; Ix_p_pos <= ISol->GetNbNodes(); ++Ix_p_pos)
	{
		std::vector<int> Ix_d_pos_vec = ISol->GetDInsertedPos(Ix_p_pos);

		for (auto Ix_d_pos : Ix_d_pos_vec)
		{
			double cost_in = ISol->GainInertionAdd(in_job_id, Ix_p_pos, Ix_d_pos);

			this->record_vec.emplace_back(Record(Ix_p_pos, Ix_d_pos, cost_in, 0.0));
		}
	}


	double k_regret_value = 0;

	int Rl = 0;
	while (Rl != k + 1)
	{
		MoveMintoLeft(this->record_vec, Rl);
		++Rl;
	}

	//std::sort(this->record_vec.begin(), this->record_vec.end(), cmp1);
	std::vector<Record>::iterator record_info = record_vec.begin();

	while (k--)		//while(k-- && record_info != record_vec.end())
	{
		++record_info;
		k_regret_value += (*record_info).InCost - record_vec.front().InCost;
	}

	return k_regret_value;
}


void RepairMethod::CheapestInsertion(Solution*& ISol, DestroyMethod* DesM)
{
	int N = ISol->GetNbJobs();
	int M = 999999;

	while (!DesM->rm_job_vec.empty())
	{
		int best_Ix_p_pos;
		int best_Ix_d_pos;
		int next_Ix;		
		double min_delta = M;
		
		for (int Ix_p_pos = 0; Ix_p_pos <= ISol->GetNbNodes(); ++Ix_p_pos)
		{
			std::vector<int> Ix_d_pos_vec = ISol->GetDInsertedPos(Ix_p_pos);

			for (auto Ix_d_pos : Ix_d_pos_vec)
			{
				for (auto x : DesM->rm_job_vec)
				{

					double cur_delta = ISol->GainInertionAdd(x, Ix_p_pos, Ix_d_pos);

					if (cur_delta < min_delta)
					{
						min_delta = cur_delta;
						next_Ix = x;
						best_Ix_p_pos = Ix_p_pos;
						best_Ix_d_pos = Ix_d_pos;
					}
				}
			}
		}		

		//Insert the cheapest job into isol;
		ISol->InsertNodeAtPosition(next_Ix + N, best_Ix_d_pos);
		ISol->InsertNodeAtPosition(next_Ix, best_Ix_p_pos);
		ISol->UpDataLoadStatus(best_Ix_p_pos, best_Ix_d_pos + 1);	//!!!
		ISol->ChangeCost(min_delta);

		/*
		std::cout << "insert jobs : " << next_Ix.first << "into : ("
			<< best_Ix_p_pos << ", " << best_Ix_d_pos << "), delta :" << min_delta
			<< std::endl;
		*/

		std::vector<int>::iterator pos_in_js = std::find(DesM->rm_job_vec.begin(), DesM->rm_job_vec.end(), next_Ix);
		DesM->rm_job_vec.erase(pos_in_js);
	}
	return;
}


void RepairMethod::RegretInsertion(Solution*& ISol, DestroyMethod* DesM)
{
	int N = ISol->GetNbJobs();
	double M = 999999.0;

	while (!DesM->rm_job_vec.empty())
	{
		std::vector<std::vector<Record>> matrix(DesM->rm_job_vec.size());

		for (int Ix_p_pos = 0; Ix_p_pos <= ISol->GetNbNodes(); ++Ix_p_pos)
		{
			std::vector<int> Ix_d_pos_vec = ISol->GetDInsertedPos(Ix_p_pos);

			for (auto Ix_d_pos : Ix_d_pos_vec)
			{
				for (int x_i = 0; x_i < DesM->rm_job_vec.size(); ++x_i)
				{		
					double delta = ISol->GainInertionAdd(DesM->rm_job_vec[x_i], Ix_p_pos, Ix_d_pos);
					matrix[x_i].emplace_back(Record(Ix_p_pos, Ix_d_pos, delta));
				}
			}
		}

		std::vector<double> regretVal(DesM->rm_job_vec.size(), 0.0);
		for (int x_i = 0; x_i < DesM->rm_job_vec.size(); ++x_i)
		{
			if (matrix[x_i].empty()) { continue; }

			if (matrix[x_i].size() > 16)
			{
				int left = 0;
				while (left <= k_step)
				{
					//move min to left
					int index = 0;
					double minInc = M;
					for (int rcd = left; rcd < matrix[x_i].size(); ++rcd)
					{
						if (matrix[x_i][rcd].InCost < minInc)
						{
							minInc = matrix[x_i][rcd].InCost;
							index = rcd;
						}
					}

					std::swap(matrix[x_i][left], matrix[x_i][index]);
					++left;
				}
			}
			else
			{
				std::sort(matrix[x_i].begin(), matrix[x_i].end(), (cmp2));
			}

			//calculate regret value
			for (int k = 1; k <= k_step; ++k)
			{
				regretVal[x_i] += (matrix[x_i][k].InCost - matrix[x_i][0].InCost);
			}
		}
		

		int best_Ix_p_pos;
		int best_Ix_d_pos;
		int bxi;
		double min_delta = M;
		double max_reg = -1;


		for (int x_i = 0; x_i < DesM->rm_job_vec.size(); ++x_i)
		{
			if (regretVal[x_i] > max_reg)
			{
				max_reg = regretVal[x_i];
				best_Ix_p_pos = matrix[x_i][0].p_d_successor[0];
				best_Ix_d_pos = matrix[x_i][0].p_d_successor[1];
				bxi = x_i;
				min_delta = matrix[x_i][0].InCost;
			}
		}
		int next_Ix = DesM->rm_job_vec[bxi];



		//Insert the cheapest job into isol;
		ISol->InsertNodeAtPosition(next_Ix + N, best_Ix_d_pos);
		ISol->InsertNodeAtPosition(next_Ix, best_Ix_p_pos);
		ISol->UpDataLoadStatus(best_Ix_p_pos, best_Ix_d_pos + 1);	//!!!
		ISol->ChangeCost(min_delta);

		/*
		std::cout << "insert jobs : " << next_Ix << " into : ("
			<< best_Ix_p_pos << ", " << best_Ix_d_pos << "), delta :" << min_delta
			<< std::endl;
		*/

		DesM->rm_job_vec.erase(DesM->rm_job_vec.begin() + bxi);

	}

	return;
}


