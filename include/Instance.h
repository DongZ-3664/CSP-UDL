#ifndef _Instance_H_
#define _Instance_H_

#include "PublicFunction.h"

class Instance
{
public:
	Instance();
	Instance(int number_requests, int crane_capacity);
	Instance(Instance* ins);
	~Instance();

	void PrintInstanceInformation();


	int Capacity;
	int InsNbJobs;
	double** Distance;
	std::vector<int> JobSet;	//JobSet = {[1,2,......,N]}, Index ~ [0, N - 1];
private:
	std::string FileName = datafile;
};



#endif // !_Instance_H_