#include "Acceptance.h"

AcceptanceMethod::AcceptanceMethod(std::string name)
{
	this->OperatorName = name;
}

AcceptanceMethod::~AcceptanceMethod()
{
}

void AcceptanceMethod::SetTemperature(double t)
{
	this->temperature = t;
}


double AcceptanceMethod::GetTemperature()
{
	return this->temperature;
}

bool AcceptanceMethod::HillClimbing(Solution* current_sol, Solution* neighbor_sol)
{


	return neighbor_sol->GetTotalCosts() < current_sol->GetTotalCosts();
}


bool AcceptanceMethod::SimulatedAnnealing(Solution* current_sol, Solution* neighbor_sol)
{
	double fc = current_sol->GetTotalCosts();
	double fn = neighbor_sol->GetTotalCosts();

	if (std::fabs(fc - fn) < __EPSILON) { return false; }		// fc - fn == 0;

	if (this->temperature == 0.0)
	{

		double tm = (fn - fc) / std::log(2);
		this->SetTemperature(tm);
	}

	std::default_random_engine e(time(0));
	std::uniform_real_distribution<double> u(0, 1);

	double cmp = std::exp((fc - fn) / this->temperature);

	return u(e) < cmp;

}

void AcceptanceMethod::Cooling()
{
	this->SetTemperature(this->cooling_rate * this->temperature);
}
