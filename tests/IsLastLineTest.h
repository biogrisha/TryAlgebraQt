#pragma once
#include "TestHelpers.h"
#include "TestGlobals.h"
#include "TestFramework.h"

namespace IsLastLineTest
{

	struct IsLastLineCase
	{
		TestFramework::TestData tst;
		std::wstring str;
		uint64_t test_child_pos = 0;
		bool exp_val = true;
	};

	inline void isLastLineTest(const TestFramework::TestData& tst, const IsLastLineCase& data)
	{
		auto cont = parse(data.str);
		bool res = MeHelpers::isLastLine(cont.get(), data.test_child_pos);
		MY_EQ(res, data.exp_val);
	}

	MYTEST(IsLastLineTest)
	{
		TestFramework::Cases<IsLastLineCase> cases(
			{
				IsLastLineCase{{"IsLastLineTest", "case1"},L"abc\nefg\n", std::wcslen(L"ab"), false},
				IsLastLineCase{{"IsLastLineTest", "case2"},L"abc\nefg\n", std::wcslen(L"abc\ne"), true},
				IsLastLineCase{{"IsLastLineTest", "case3"},L"abc\nefg\n\n", std::wcslen(L"abc\nefg\n"), true},
				IsLastLineCase{{"IsLastLineTest", "case4"},L"abc\nefg\n\n", std::wcslen(L"abc\nef"), false},
			}, & isLastLineTest);
	}
}