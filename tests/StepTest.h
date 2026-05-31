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
		MY_EQ(tst_path.value(), exp_path.value());
	}

	MYTEST(StepDownTest)
	{
		TestFramework::Cases<StepTestCase, true> cases(
			{
				StepTestCase{{"StepDownTest", "case1"},L"a\\ft\\A\\{\\,\\}bc\nefg\n", 
				std::wcslen(L"a\\ft\\A\\{"), std::wcslen(L"a\\ft\\A\\{\\,")},

				StepTestCase{{"StepDownTest", "case2"},L"a\\ft\\A\\{\\,\\}bc\nefg\n", 
				std::wcslen(L"a\\ft\\A\\{\\,"), std::wcslen(L"a\\ft\\A\\{\\,\\}")},

				StepTestCase{{"StepDownTest", "case3"},L"a\\ft\\A\\{dvd\\,vdv\\}bc\nefg\n", 
				std::wcslen(L"a\\ft\\A\\{d"), std::wcslen(L"a\\ft\\A\\{dvd\\,")},

				StepTestCase{{"StepDownTest", "case4"},L"a\\ft\\A\\{dvd\\,vdv\\}bc\nefg\n",
				std::wcslen(L"a\\ft\\A\\{"), std::wcslen(L"a\\ft\\A\\{dvd\\,")},

				StepTestCase{{"StepDownTest", "case5"},L"a\\ft\\A\\{dvd\\,vdv\\}bc\nefg\n",
				std::wcslen(L"a\\ft\\A\\{dvd\\,vd"), std::wcslen(L"a\\ft\\A\\{dvd\\,vdv\\}")},

				StepTestCase{{"StepDownTest", "case6"},L"a\\ft\\A\\{dvd\\,vdv\\}bc\nefg\n",
				std::wcslen(L"a\\ft\\A\\{dvd\\,vdv"), std::wcslen(L"a\\ft\\A\\{dvd\\,vdv\\}")},

				StepTestCase{{"StepDownTest", "case7"},L"abc\nefg",
				std::wcslen(L"ab"), std::wcslen(L"abc\nef")},
			}, &stepDownTest);
	}
}
