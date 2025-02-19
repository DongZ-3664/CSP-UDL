#include "PublicFunction.h"


double Speed(int l)
{
	//return 1 + 0.5 * l; //输入0 返回1，输入1 返回1.5，输入2，返回2.0
	//return l + 2;

	if (l == 0) return 1;

	return 1.5 < l ? Multiplier_L2 : Multiplier_L1;

}

int RandNum(int l, int r) //random generate a integer in [l, r]
{
	std::mt19937 rng;
	rng.seed(std::random_device()());
	std::uniform_int_distribution<std::mt19937::result_type> dist6(l, r);
	return dist6(rng);
}

double RandDouble(double l, double r) //random generate a fraction in [l, r]
{
	std::mt19937 rng;
	rng.seed(std::random_device()());
	std::uniform_real_distribution<double> dist6(l, r);
	return dist6(rng);
}

std::string itos(const int i)
{
	std::stringstream s;
	s << i;
	return s.str();
}



bool myself::createDirectory(std::string folder)
{
	std::string folder_builder;
	std::string sub;
	sub.reserve(folder.size());
	for (auto it = folder.begin(); it != folder.end(); ++it) {
		//cout << *(folder.end()-1) << endl;
		const char c = *it;
		sub.push_back(c);
		if (c == '/' || it == folder.end() - 1) {
			folder_builder.append(sub);
			if (0 != ::access(folder_builder.c_str(), 0)) {
				// this folder not exist
				if (0 != ::mkdir(folder_builder.c_str(), 00777)) {
					// create failed
					return false;
				}
			}
			sub.clear();
		}
	}
	return true;
}
