#ifndef _ALNS_H_
#define _ALNS_H_

#include "RelocateMethod.h"
#include "InitialSolutionCreator.h"
#include "DestroyMethod.h"
#include "RepairMethod.h"
#include "ExchangeMethod.h"
#include "Acceptance.h"
#include "ShakeMethod.h"
#include "TabuSolutionList.h"
#include "TabuOperatorList.h"
#include "SolutionPool.h"
#include "MasterProblem.h"




double _TABU_ALNS	(	int n,
						int& best_sol_its,
	                    Instance*& test_instance,
						SolutionPool* sol_pool = NULL
	                    );


double _ALNS_RESTRICTED_MASTER	(	int n	);





#endif // !_ALNS_H_

