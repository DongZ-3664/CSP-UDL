#include "Instance.h"

Instance::Instance(int number_jobs, int crane_capacity)
{
	Capacity = crane_capacity;
	InsNbJobs = number_jobs;
	JobSet.resize(number_jobs);
	for (auto i = 0u; i < JobSet.size(); i++)
	{
		JobSet[i] = i + 1;	//JobSet[0,1,.....N-1] = {1,2,.....,N};
	}

	CreateArray_02<double>(Distance, 2 * InsNbJobs, 2 * InsNbJobs);
	ReadData_02<double>(FileName, Distance, 2 * InsNbJobs, 2 * InsNbJobs);
}

Instance::Instance(Instance* ins)
{
	this->Capacity = ins->Capacity;
	this->InsNbJobs = ins->InsNbJobs;
	this->JobSet = ins->JobSet;
	this->FileName = ins->FileName;
	CreateArray_02<double>(this->Distance, 2 * InsNbJobs, 2 * InsNbJobs);
	for (int i = 1; i <= 2 * InsNbJobs; ++i)
		for (int j = 1; j <= 2 * InsNbJobs; ++j)
			this->Distance[i][j] = ins->Distance[i][j];

}

Instance::Instance()
{

}

Instance::~Instance()
{
	Release_02<double>(Distance, 2 * InsNbJobs);
}

void Instance::PrintInstanceInformation()
{
	std::cerr << "Information about the current instance  : " << std::endl;
	std::cerr << "The number of jobs : " << this->InsNbJobs << std::endl;
	std::cerr << "Crane capacity : " << this->Capacity << std::endl;
	std::cerr << "Distance matrix : " << std::endl;
	for (auto i = 1u; i <= 2 * InsNbJobs; i++)
	{
		for (auto j = 1u; j <= 2 * InsNbJobs; j++)
		{
			std::cout << this->Distance[i][j] << " ";
		}
		std::cout << std::endl;
	}
}
