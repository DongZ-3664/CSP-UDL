#include "ShakeMethod.h"

ShakeMethod::ShakeMethod(std::string name)
{
	this->OperatorName = name;
}

ShakeMethod::~ShakeMethod()
{

}

void ShakeMethod::ShuffBlock(Solution*& sol)
{
	std::vector<std::vector<int>> res;

	int start = 0;

	while (start < sol->GetNbNodes() - 1)
	{
		int i = start;
		std::vector<int> block;

		if (sol->LoadAtPos(i) == 1 && sol->LoadAtPos(i + 1) == 0)
		{
			while (i < sol->GetNbNodes() && sol->LoadAtPos(i) == 1 && sol->LoadAtPos(i + 1) == 0)
			{
				block.emplace_back(sol->NodeAt(i));
				block.emplace_back(sol->NodeAt(i + 1));
				i += 2;
			}

			res.emplace_back(block);
			start = i;
			continue;
		}

		while (sol->LoadAtPos(i))
		{
			block.emplace_back(sol->NodeAt(i));
			++i;
		}
		block.emplace_back(sol->NodeAt(i));
		res.emplace_back(block);
		start = ++i;
	}

	std::srand(time(NULL));
	std::random_shuffle(res.begin(), res.end());

	start = 0;
	std::vector<int> sol_path;
	sol_path.resize(sol->GetNbNodes());
	while (!res.empty())
	{
		for (auto i = 0u; i < res.back().size(); ++i)
		{
			sol_path[start++] = res.back()[i];
		}
		res.pop_back();
	}

	Instance* ins = sol->GetInstance();
	sol->~Solution();
	sol = new Solution(sol_path, ins);
}
