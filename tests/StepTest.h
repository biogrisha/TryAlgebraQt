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
		TestFramework::Cases<StepTestCase, false> cases(
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

			StepTestCase{ {"StepDownTest", "case8"},L"abc\nefg",
			std::wcslen(L""), std::wcslen(L"abc\n") },

			StepTestCase{ {"StepDownTest", "case9"},L"abc\nefg",
			std::wcslen(L"abc"), std::wcslen(L"abc\nefg") },

			StepTestCase{ {"StepDownTest", "case10"},L"abc\nefg",
			std::wcslen(L"abc\n"), std::wcslen(L"abc\n") },

			StepTestCase{ {"StepDownTest", "case11"},L"abc\nefg",
			std::wcslen(L"abc\nefg"), std::wcslen(L"abc\nefg") },
		}, &stepDownTest);

	}

	inline void stepUpTest(const TestFramework::TestData& tst, const StepTestCase& data)
	{
		TestGlobals::ContCalculated fx(data.str);
		auto tst_path = MeHelpers::textPosToMePath(fx.cont.get(), data.test_pos);
		auto exp_path = MeHelpers::textPosToMePath(fx.cont.get(), data.exp_pos);
		fx.cont->step(StepDir::up, StepFrom::none, tst_path.value());
		MY_EQ(tst_path.value(), exp_path.value());
	}

	MYTEST(StepUpTest)
	{
		TestFramework::Cases<StepTestCase, false> cases(
			{
				StepTestCase{{"StepUpTest", "case1"},L"a\\ft\\A\\{\\,\\}bc\nefg\n",
				std::wcslen(L"a\\ft\\A\\{"), std::wcslen(L"a")},

				StepTestCase{{"StepUpTest", "case2"},L"a\\ft\\A\\{\\,\\}bc\nefg\n",
				std::wcslen(L"a\\ft\\A\\{\\,"), std::wcslen(L"a\\ft\\A\\{")},

				StepTestCase{{"StepUpTest", "case3"},L"a\\ft\\A\\{dvd\\,vdv\\}bc\nefg\n",
				std::wcslen(L"a\\ft\\A\\{d"), std::wcslen(L"a")},

				StepTestCase{{"StepUpTest", "case4"},L"a\\ft\\A\\{dvd\\,vdv\\}bc\nefg\n",
				std::wcslen(L"a\\ft\\A\\{"), std::wcslen(L"a")},

				StepTestCase{{"StepUpTest", "case5"},L"a\\ft\\A\\{dvd\\,vdv\\}bc\nefg\n",
				std::wcslen(L"a\\ft\\A\\{dvd\\,vd"), std::wcslen(L"a\\ft\\A\\{dvd")},

				StepTestCase{{"StepUpTest", "case6"},L"a\\ft\\A\\{dvd\\,vdv\\}bc\nefg\n",
				std::wcslen(L"a\\ft\\A\\{dvd\\,vdv"), std::wcslen(L"a\\ft\\A\\{dvd")},

				StepTestCase{{"StepUpTest", "case7"},L"aaa\naaa",
				std::wcslen(L"aaa\naaa"), std::wcslen(L"aaa")},

				StepTestCase{{"StepUpTest", "case8"},L"aaa\naaa",
				std::wcslen(L"aaa\naa"), std::wcslen(L"aa")},

				StepTestCase{{"StepUpTest", "case9"},L"aaa\naaaaa",
				std::wcslen(L"aaa\naaaa"), std::wcslen(L"aaa")},

				StepTestCase{{"StepUpTest", "case10"},L"aaa\naaaaa",
				std::wcslen(L"aaa\naaaaa"), std::wcslen(L"aaa")},

				StepTestCase{{"StepUpTest", "case11"},L"iiMii\niiiii",
				std::wcslen(L"iiMii\niii"), std::wcslen(L"ii")},

				StepTestCase{{"StepUpTest", "case12"},L"iii\niiii\niii",
				std::wcslen(L"iii\niiii"), std::wcslen(L"iii")},

			}, &stepUpTest);

	}
}
