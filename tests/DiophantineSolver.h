#pragma once

#include "TestHelpers.h"
#include "TestGlobals.h"
#include <TRS/PatternMatching.h>
#include <iostream>
#include <span>
namespace DiophantineSolver
{
	struct Equation
	{
		std::vector<int> positiveCoefs;
		std::vector<int> negativeCoefs;
	};

	struct Probe
	{
		std::vector<int> positiveVars;
		std::vector<int> negativeVars;
	};

	inline Equation equation;
	inline std::vector<Probe> P;
	inline std::vector<Probe> M;
	inline std::vector<Probe> Q;

	inline int d(const Probe& p)
	{
		int Sum = 0;
		for (int i = 0; i < equation.positiveCoefs.size(); ++i)
		{
			Sum += equation.positiveCoefs[i] * p.positiveVars[i];
		}


		for (int i = 0; i < equation.negativeCoefs.size(); ++i)
		{
			Sum -= equation.negativeCoefs[i] * p.negativeVars[i];
		}
		return Sum;
	}

	inline bool minimal(const std::vector<Probe>& among, const Probe& p)
	{
		if (among.empty())
		{
			return true;
		}
		for (auto& testP : among)
		{
			for (int i = 0; i < testP.positiveVars.size(); ++i)
			{
				if (p.positiveVars[i] < testP.positiveVars[i])
				{
					return true;
				}
			}
			for (int i = 0; i < testP.negativeVars.size(); ++i)
			{
				if (p.negativeVars[i] < testP.negativeVars[i])
				{
					return true;
				}
			}
		}
		return false;
	}
	MYTEST(DiophantineSolver)
	{
		equation = { {1,3,5},{10} };
		P = {
			Probe{{1,0,0}, {0}},
			Probe{{0,1,0}, {0}},
			Probe{{0,0,1}, {0}},
		};

		while (!P.empty())
		{
			for (auto& p : P)
			{
				int posMax = p.positiveVars.size() - 1;
				for (; posMax >= 0; --posMax)
				{
					if (p.positiveVars[posMax] > 0)
					{
						break;
					}
				}
				posMax = std::max(0, posMax);
				int negMax = p.negativeVars.size() - 1;
				for (; negMax >= 0; --negMax)
				{
					if (p.negativeVars[negMax] > 0)
					{
						negMax++;
						break;
					}
				}
				negMax = std::max(0, negMax);

				int dRes = d(p);
				if (dRes < 0)
				{
					for (int i = posMax; i < equation.positiveCoefs.size(); ++i)
					{
						Probe pCopy = p;
						pCopy.positiveVars[i] += 1;
						Q.push_back(pCopy);
					}
				}
				else if (dRes > 0)
				{
					for (int i = negMax; i < equation.negativeCoefs.size(); ++i)
					{
						Probe pCopy = p;
						pCopy.negativeVars[i] += 1;
						Q.push_back(pCopy);
					}
				}
			}

			P.clear();
			for (auto& q : Q)
			{
				if (d(q) != 0 && minimal(M, q))
				{
					P.push_back(q);
				}
			}
			for (auto& q : Q)
			{
				if (d(q) == 0)
				{
					M.push_back(q);
				}
			}
			Q.clear();

		}

		for (auto m : M)
		{
			std::cout << "\n[";
			for (auto pos : m.positiveVars)
			{
				std::cout << pos << " ";
			}
			std::cout << "|";
			for (auto neg : m.negativeVars)
			{
				std::cout << neg << " ";
			}
			std::cout << "]";
		}
	}
}