#ifndef Exception_H_
#define Exception_H_

#include "PublicFunction.h"

class IllegalParameters
{
public:
	IllegalParameters() :message("illegal parameters value !") {};

	IllegalParameters(std::string e) :message(e) {};
	~IllegalParameters() {};

	void GetErrorMessage()
	{
		std::cerr << this->message << std::endl;
	}

private:
	std::string message;
};


#endif // !Exception