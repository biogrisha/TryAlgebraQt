#pragma once
#include "TestFramework.h"
#include <Actions/SimpleTree.h>

namespace MeSimpleToStrTest
{
	using namespace TryAlgebraCore;

	struct MeSimpleToStrGenericCase
	{
		TestFramework::TestData tst;
		std::wstring str;
	};


	inline void test(const TestFramework::TestData& tst, const MeSimpleToStrGenericCase& tc)
	{
		auto res = SimpleMe::fromStr(tc.str);
		MY_EQ(SimpleMe::toStr(res), tc.str);
	}

	MYTEST(MeParserGenericTest)
	{
		TestFramework::Cases<MeSimpleToStrGenericCase, TestFramework::CasesBehavior::All> cases(
			{
				{{"MeParserGenericTest", "1"}, L"abcd"},
				{{"MeParserGenericTest", "2"}, L"\\vr\\1mlt\\{x\\}+\\vr\\1mlt\\{y\\}="},
				{{"MeParserGenericTest", "3"}, L"ab+ac+dsa\\ft\\∫\\{\\,\\nl\\\\\\nl\\\\\\}\\pst\\\\"},
				{{"MeParserGenericTest", "4"}, L"\\pst\\\\"},
			}, &test);

	}

}
