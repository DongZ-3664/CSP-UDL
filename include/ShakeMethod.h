#ifndef _ShakeMethod_H_
#define _ShakeMethod_H_

#include "Solution.h"

class ShakeMethod
{
public:
	ShakeMethod(std::string name);
	~ShakeMethod();


	void ShuffBlock(Solution*& sol);
private:
	std::string OperatorName;
};


#endif // !_ShakeMethod_H_

