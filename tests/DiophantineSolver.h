#pragma once

#include "TestHelpers.h"
#include "TestGlobals.h"
#include <TRS/PatternMatching.h>
#include <iostream>
#include <span>

namespace DiophantineSolver
{
	struct Var
	{
		int value = 0;
	};

	struct EqVar
	{
		Var* var = nullptr;
		int coef = 0;
	};

	struct Equation
	{
		std::vector<EqVar> prevVars;
		std::vector<EqVar> initVars;
		int rhs = 0;

		std::unique_ptr<Equation> next;
	};

	enum class Status
	{
		finished,
		exceeded,
		succeeded
	};

	Status solve(int pos, Equation& eq, int remainder)
	{
		if (pos == eq.initVars.size())
		{
			if (remainder == 0)
			{
				//add variables were subtracted from rhs and it became zero
				//means that we found a solution
				{
					//print solution
					int sum = 0;
					for (auto val : eq.initVars)
					{
						sum += val.var->value * val.coef;
						std::cout << val.var->value << " ";
					}
					std::cout << "    " << sum;
					std::cout << "\n";

					std::cout << "next eq \n";
				}
				if (eq.next)
				{
					//has next equation
					//subtract variables initialized by previous equations
					int sum = 0;
					for (auto& var : eq.next->prevVars)
					{
						sum += var.var->value * var.coef;
					}
					solve(0, *eq.next.get(), eq.next->rhs - sum);
				}
				return Status::succeeded;
			}
			else if (remainder > 0)
			{
				// haven't found solution yet, try next iteration
				return Status::succeeded;
			}
		}

		if (remainder <= 0)
		{
			return Status::exceeded;
		}

		for (int i = 1; ; ++i)
		{
			eq.initVars[pos].var->value = i;
			auto status = solve(pos + 1, eq, remainder - (i * eq.initVars[pos].coef));
			if (status == Status::finished)
			{
				return Status::finished;
			}
			else if (status == Status::exceeded)
			{
				break;
			}
		}
		return Status::succeeded;
	}


	MYTEST(DiophantineSolver)
	{
		Equation eq;
		std::unique_ptr<Var> v1 = std::make_unique<Var>();
		std::unique_ptr<Var> v2 = std::make_unique<Var>();
		std::unique_ptr<Var> v3 = std::make_unique<Var>();
		std::unique_ptr<Var> v4 = std::make_unique<Var>();
		std::unique_ptr<Var> v5 = std::make_unique<Var>();
		std::unique_ptr<Var> v6 = std::make_unique<Var>();
		eq.initVars = {
			{v1.get(), 1},
			{v2.get(), 2},
			{v3.get(), 2},
			{v4.get(), 1},
		};
		eq.rhs = 9;
		eq.next = std::make_unique<Equation>();
		auto next = eq.next.get();
		next->prevVars =
		{
			{v1.get(), 1},
			{v3.get(), 1}
		};
		next->initVars = {
			{v5.get(), 2},
			{v6.get(), 3}
		};
		next->rhs = 12;
		solve(0, eq, eq.rhs);
	}
}