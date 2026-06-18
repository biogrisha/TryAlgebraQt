#pragma once
#include "TestHelpers.h"
#include "TestGlobals.h"
#include "TestFramework.h"
#include <TRS/PatternMatching.h>

namespace VariatorTest
{

	struct VariatorTestCase
	{
		TestFramework::TestData tst;
		std::wstring str;
		uint64_t test_child_pos = 0;
		bool exp_val = true;
	};
	inline void print(TryAlgebraCore::Trs::Variator& var)
	{
		for (auto i : var.m_sizes)
		{
			std::cout << i;
			std::cout << " ";
		}
		std::cout << "\n";
	}
	inline void variatorTest(const TestFramework::TestData& tst, const VariatorTestCase& data)
	{
		TryAlgebraCore::Trs::Variator var(5, 6);
		while(true)
		{
			print(var);
			
			if (var.step() == 1)
			{
				break;
			}
		}
		print(var);
	}

	MYTEST(VariatorTest)
	{
		TestFramework::Cases<VariatorTestCase> cases(
			{
				VariatorTestCase{{"VariatorTest", "case1"}},
			}, &variatorTest);
	}
}