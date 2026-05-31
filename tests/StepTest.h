#pragma once
#include "TestHelpers.h"
#include "TestGlobals.h"
#include "TestFramework.h"

namespace StepTest
{
	
	struct StepTestCase
	{
		TestFramework::TestData tst;
		std::wstring str;
		uint64_t test_pos = 0;
		uint64_t exp_pos = 0;
	};

	inline void stepDownTest(const TestFramework::TestData& tst, const StepTestCase& data)
	{
		TestGlobals::ContCalculated fx(data.str);
		auto tst_path = MeHelpers::textPosToMePath(fx.cont.get(), data.test_pos);
		auto exp_path = MeHelpers::textPosToMePath(fx.cont.get(), data.exp_pos);
		fx.cont->step(StepDir::down, StepFrom::none, tst_path.value());
		if (!tst.eq(tst_path, exp_path))
		{
			return;
		}
	}

	MYTEST(StepDownTest)
	{
		TestFramework::Cases<StepTestCase> cases(
			{
				StepTestCase{{"Step Down Test", "case1"},L"abc\nefg\n", std::wcslen(L"ab"), std::wcslen(L"abc\nef")},
				StepTestCase{{"Step Down Test", "case2"},L"abc\nefg\n", std::wcslen(L"ab"), std::wcslen(L"abc\nef")},
				StepTestCase{{"Step Down Test", "case3"},L"abc\nefg\n", std::wcslen(L"ab"), std::wcslen(L"abc\nef")},
			}, & stepDownTest);
	}
}
