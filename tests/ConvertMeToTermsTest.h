#pragma once
#include "TestHelpers.h"
#include "TestGlobals.h"
#include "TestFramework.h"
#include <TRS/PatternMatching.h>
#include <TRS/Trs.h>
#include <Me/include/MeGlobals.h>

namespace ConvertMeToTermsTest
{
	using namespace TryAlgebraCore::Trs;
	using namespace TestHelpers;
	struct ConvertMeToTermsCase
	{
		ConvertMeToTermsCase(ConvertMeToTermsCase&&) = default;
		ConvertMeToTermsCase& operator=(ConvertMeToTermsCase&&) = default;
		ConvertMeToTermsCase() = default;
		~ConvertMeToTermsCase()
		{
			for (auto t : expTerms)
			{
				deleteRecursive(t);
			}
		}

		TestFramework::TestData tst;
		std::wstring str;
		std::vector<Term*> expTerms;
	};

	inline void test(const TestFramework::TestData& tst, const ConvertMeToTermsCase& data)
	{
		/*auto cont = TestHelpers::parse(data.str);
		std::vector<Term*> result;
		convertMeToTerms(cont->getChildren(), result, nullptr);
		MY_EQ(TestHelpers::compareTerms(result, data.expTerms), true);*/
	}

	MYTEST(ConvertMeToTermsTest)
	{
		TestFramework::Cases<ConvertMeToTermsCase> cases({}, &test);
		{
			ConvertMeToTermsCase case1;
			case1.tst = { "ConvertMeToTermsTest", "case1" };
			case1.str = L"abc";

			makeTerm(L"a", case1.expTerms);
			makeTerm(L"b", case1.expTerms);
			makeTerm(L"c", case1.expTerms);
			cases += std::move(case1);
		}
		{
			ConvertMeToTermsCase case1;
			case1.tst = { "ConvertMeToTermsTest", "case2" };
			case1.str = L"ab\\ft\\A\\{abc\\,efg\\}efg";

			makeTerm(L"a", case1.expTerms);
			makeTerm(L"b", case1.expTerms);
			auto ft = makeTerm(MeNames::from_to + L"_A", case1.expTerms);
			auto cont1 = makeTerm(MeNames::cont, ft->children, ft);
			makeTerm(L"a", cont1->children, cont1);
			makeTerm(L"b", cont1->children, cont1);
			makeTerm(L"c", cont1->children, cont1);
			auto cont2 = makeTerm(MeNames::cont, ft->children, ft);
			makeTerm(L"e", cont2->children, cont2);
			makeTerm(L"f", cont2->children, cont2);
			makeTerm(L"g", cont2->children, cont2);
			makeTerm(L"e", case1.expTerms);
			makeTerm(L"f", case1.expTerms);
			makeTerm(L"g", case1.expTerms);
			cases += std::move(case1);
		}
		{
			ConvertMeToTermsCase case1;
			case1.tst = { "ConvertMeToTermsTest", "case3" };
			case1.str = L"\\vr\\\\{X\\}";

			auto ft = makeTerm(MeNames::variable, case1.expTerms);
			auto cont1 = makeTerm(MeNames::cont, ft->children, ft);
			makeTerm(L"X", cont1->children, cont1);
			cases += std::move(case1);
		}
	}
}