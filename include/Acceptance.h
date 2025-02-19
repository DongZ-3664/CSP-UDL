#ifndef _Acceptance_H_
#define _Acceptance_H_

#include "Solution.h"
#include "PublicFunction.h"

class AcceptanceMethod
{
public:
	AcceptanceMethod(std::string name);
	~AcceptanceMethod();


	void SetTemperature(double t);
	double GetTemperature();
	void Cooling();

	bool SimulatedAnnealing(Solution* current_sol, Solution* neighbor_sol);
	bool HillClimbing(Solution* current_sol, Solution* neighbor_sol);

private:
	std::string OperatorName;

	double temperature = 0;

	const double cooling_rate = _COOLING_RATE;
};



#endif // !_Acceptance_H_
