#pragma once
#include "TestHelpers.h"
#include "TestGlobals.h"
#include <TRS/PatternMatching.h>

namespace VariatorTest
{
	using namespace TryAlgebraCore::Trs;
	struct OffsetsToSizesCase
	{
		TestFramework::TestData tst;
		std::vector<int> offsets;
		std::vector<int> sizes;
		int sum = 0;
	};

	inline void offsetsToSizesTest(const TestFramework::TestData& tst, const OffsetsToSizesCase& data)
	{
		auto res = TryAlgebraCore::Trs::Variator::generateSizes(data.offsets, data.sum);
		MY_EQ(res, data.sizes);
	}

	struct VariatorTestCase
	{
		TestFramework::TestData tst;
		int sum = 0;
		std::vector<int> offsets;
		std::vector<int> offsetsRes;

	};
	inline TermRawSh makeTermRaw(TermRaw* parent, const std::wstring& label, bool variable = false)
	{
		auto res = std::make_shared<TermRaw>();
		res->label = label;
		res->variable = variable;
		res->parent = parent;
		return res;
	}

	inline void variatorTest(const TestFramework::TestData& tst, const VariatorTestCase& data)
	{
		Variator var(data.offsets.size() + 1, data.sum);
		var.m_offsets = data.offsets;
		var.step();
		MY_EQ(var.m_offsets, data.offsetsRes);
	}

	struct StateCompareBoundariesCase
	{
		TestFramework::TestData tst;
		std::vector<TermRawSh> pat;
		std::vector<TermRawSh> terms;
		bool expRes = true;

		int variablesStartPat = 0;
		int variablesEndPat = 0;
		int variablesStartTerms = 0;
		int variablesEndTerms = 0;
	};

	inline void stateCompareBoundariesTest(const TestFramework::TestData& tst, const StateCompareBoundariesCase& data)
	{
		StateCompareBoundariesCase dataCopy = data;
		PatternMatcher::State state(dataCopy.pat, dataCopy.terms);
		bool res = state.compBoundaries();
		MY_EQ(res, data.expRes);
		if (res)
		{
			MY_EQ(state.variablesStartPat, data.variablesStartPat);
			MY_EQ(state.variablesStartTerms, data.variablesStartTerms);
			MY_EQ(state.variablesEndPat, data.variablesEndPat);
			MY_EQ(state.variablesEndTerms, data.variablesEndTerms);
		}
	}

	MYTEST(VariatorTest)
	{

		//--------------variatorTest-------------
		
		TestFramework::Cases<OffsetsToSizesCase, TestFramework::CasesBehavior::None> offsetsToSizesCases(
			{}, &offsetsToSizesTest);

		TestFramework::Cases<VariatorTestCase, TestFramework::CasesBehavior::All> variatorTestCases(
			{}, &variatorTest);

		TestFramework::Cases<StateCompareBoundariesCase, TestFramework::CasesBehavior::None> cases2(
			{}, &stateCompareBoundariesTest
		);


		//--------------offsetsToSizesTest
		{
			OffsetsToSizesCase case1;
			case1.tst = { "OffsetsToSize", "case1" };
			case1.sum = 5;
			case1.offsets = { 1,2 };
			case1.sizes = { 1,1,3 };
			offsetsToSizesCases += std::move(case1);
		}
		{
			OffsetsToSizesCase case1;
			case1.tst = { "OffsetsToSize", "case2" };
			case1.sum = 5;
			case1.offsets = { 1,2,3 };
			case1.sizes = { 1,1,1,2 };
			offsetsToSizesCases += std::move(case1);
		}
		{
			OffsetsToSizesCase case1;
			case1.tst = { "OffsetsToSize", "case3" };
			case1.sum = 6;
			case1.offsets = { 2,3,5 };
			case1.sizes = { 2,1,2,1 };
			offsetsToSizesCases += std::move(case1);
		}

		//--------------variatorTest----------------------

		{
			VariatorTestCase case1;
			case1.tst = { "variatorTest", "case1" };
			case1.sum = 5;
			case1.offsets = { 1,2,3 };
			case1.offsetsRes = { 1,2,4 };
			variatorTestCases += std::move(case1);
		}


		//--------------stateCompareBoundariesTest-------------

		//-----case1
		{
			StateCompareBoundariesCase st;
			st.tst = { "CompBoundaries", "case1" };


			{
				st.terms.push_back(makeTermRaw(nullptr, L"a"));
				st.terms.push_back(makeTermRaw(nullptr, L"b"));
				st.terms.push_back(makeTermRaw(nullptr, L"-v1"));
				st.terms.push_back(makeTermRaw(nullptr, L"-v2"));
				st.terms.push_back(makeTermRaw(nullptr, L"-v2"));
				st.terms.push_back(makeTermRaw(nullptr, L"-v2"));
				st.terms.push_back(makeTermRaw(nullptr, L"-v3"));
				st.terms.push_back(makeTermRaw(nullptr, L"-v3"));
				st.terms.push_back(makeTermRaw(nullptr, L"-v3"));
				st.terms.push_back(makeTermRaw(nullptr, L"c"));

				auto b = st.terms[1].get();
				b->children.push_back(makeTermRaw(b, L"b1"));
				b->children.push_back(makeTermRaw(b, L"b2"));
				b->children.push_back(makeTermRaw(b, L"b3"));
			}

			{
				st.pat.push_back(makeTermRaw(nullptr, L"a"));
				st.pat.push_back(makeTermRaw(nullptr, L"b"));
				st.pat.push_back(makeTermRaw(nullptr, L"v1", true));
				st.pat.push_back(makeTermRaw(nullptr, L"v2", true));
				st.pat.push_back(makeTermRaw(nullptr, L"v3", true));
				st.pat.push_back(makeTermRaw(nullptr, L"c"));

				auto b = st.pat[1].get();
				b->children.push_back(makeTermRaw(b, L"b1"));
				b->children.push_back(makeTermRaw(b, L"b2"));
				b->children.push_back(makeTermRaw(b, L"b3"));
			}

			st.expRes = true;
			st.variablesStartPat = 2;
			st.variablesEndPat = 5;
			st.variablesStartTerms = 2;
			st.variablesEndTerms = 9;

			cases2 += std::move(st);
		}
		//---------case2
		{
			StateCompareBoundariesCase st;
			st.tst = { "CompBoundaries", "case2" };


			{
				st.terms.push_back(makeTermRaw(nullptr, L"a"));
				st.terms.push_back(makeTermRaw(nullptr, L"b"));
				st.terms.push_back(makeTermRaw(nullptr, L"-v1"));
				st.terms.push_back(makeTermRaw(nullptr, L"-v2"));
				st.terms.push_back(makeTermRaw(nullptr, L"-v2"));
				st.terms.push_back(makeTermRaw(nullptr, L"-v2"));
				st.terms.push_back(makeTermRaw(nullptr, L"-v3"));
				st.terms.push_back(makeTermRaw(nullptr, L"-v3"));
				st.terms.push_back(makeTermRaw(nullptr, L"-v3"));

				auto b = st.terms[1].get();
				b->children.push_back(makeTermRaw(b, L"b1"));
				b->children.push_back(makeTermRaw(b, L"b2"));
				b->children.push_back(makeTermRaw(b, L"b3"));
			}

			{
				st.pat.push_back(makeTermRaw(nullptr, L"a"));
				st.pat.push_back(makeTermRaw(nullptr, L"b"));
				st.pat.push_back(makeTermRaw(nullptr, L"v1", true));
				st.pat.push_back(makeTermRaw(nullptr, L"v2", true));
				st.pat.push_back(makeTermRaw(nullptr, L"v3", true));

				auto b = st.pat[1].get();
				b->children.push_back(makeTermRaw(b, L"b1"));
				b->children.push_back(makeTermRaw(b, L"b2"));
				b->children.push_back(makeTermRaw(b, L"b3"));
			}

			st.expRes = true;
			st.variablesStartPat = 2;
			st.variablesStartTerms = 2;
			st.variablesEndPat = 5;
			st.variablesEndTerms = 9;
			cases2 += std::move(st);
		}

		//-----case3
		{
			StateCompareBoundariesCase st;
			st.tst = { "CompBoundaries", "case3" };


			{
				st.terms.push_back(makeTermRaw(nullptr, L"-v1"));
				st.terms.push_back(makeTermRaw(nullptr, L"-v2"));
				st.terms.push_back(makeTermRaw(nullptr, L"-v2"));
				st.terms.push_back(makeTermRaw(nullptr, L"-v2"));
				st.terms.push_back(makeTermRaw(nullptr, L"-v3"));
				st.terms.push_back(makeTermRaw(nullptr, L"-v3"));
				st.terms.push_back(makeTermRaw(nullptr, L"-v3"));
				st.terms.push_back(makeTermRaw(nullptr, L"c"));

			}

			{
				st.pat.push_back(makeTermRaw(nullptr, L"v1", true));
				st.pat.push_back(makeTermRaw(nullptr, L"v2", true));
				st.pat.push_back(makeTermRaw(nullptr, L"v3", true));
				st.pat.push_back(makeTermRaw(nullptr, L"c"));
			}

			st.expRes = true;
			st.variablesStartPat = 0;
			st.variablesEndPat = 3;
			st.variablesStartTerms = 0;
			st.variablesEndTerms = 7;

			cases2 += std::move(st);
		}

		//-----case4
		{
			StateCompareBoundariesCase st;
			st.tst = { "CompBoundaries", "case4" };


			{
				st.terms.push_back(makeTermRaw(nullptr, L"-v1"));
				st.terms.push_back(makeTermRaw(nullptr, L"-v2"));
				st.terms.push_back(makeTermRaw(nullptr, L"-v2"));
				st.terms.push_back(makeTermRaw(nullptr, L"-v2"));
				st.terms.push_back(makeTermRaw(nullptr, L"-v3"));
				st.terms.push_back(makeTermRaw(nullptr, L"-v3"));
				st.terms.push_back(makeTermRaw(nullptr, L"-v3"));

			}

			{
				st.pat.push_back(makeTermRaw(nullptr, L"v1", true));
				st.pat.push_back(makeTermRaw(nullptr, L"v2", true));
				st.pat.push_back(makeTermRaw(nullptr, L"v3", true));
			}

			st.expRes = true;
			st.variablesStartPat = 0;
			st.variablesEndPat = 3;
			st.variablesStartTerms = 0;
			st.variablesEndTerms = 7;

			cases2 += std::move(st);
		}

		//-----case5
		{
			StateCompareBoundariesCase st;
			st.tst = { "CompBoundaries", "case5" };


			{
				st.terms.push_back(makeTermRaw(nullptr, L"a"));
				st.terms.push_back(makeTermRaw(nullptr, L"b"));
				st.terms.push_back(makeTermRaw(nullptr, L"-v1"));
				st.terms.push_back(makeTermRaw(nullptr, L"-v2"));
				st.terms.push_back(makeTermRaw(nullptr, L"-v2"));
				st.terms.push_back(makeTermRaw(nullptr, L"-v2"));
				st.terms.push_back(makeTermRaw(nullptr, L"-v3"));
				st.terms.push_back(makeTermRaw(nullptr, L"-v3"));
				st.terms.push_back(makeTermRaw(nullptr, L"-v3"));
				st.terms.push_back(makeTermRaw(nullptr, L"c"));

				auto b = st.terms[1].get();
				b->children.push_back(makeTermRaw(b, L"b1"));
				b->children.push_back(makeTermRaw(b, L"b2"));
				b->children.push_back(makeTermRaw(b, L"b3"));
			}

			{
				st.pat.push_back(makeTermRaw(nullptr, L"a"));
				st.pat.push_back(makeTermRaw(nullptr, L"b"));
				st.pat.push_back(makeTermRaw(nullptr, L"v1", true));
				st.pat.push_back(makeTermRaw(nullptr, L"v2", true));
				st.pat.push_back(makeTermRaw(nullptr, L"v3", true));
				st.pat.push_back(makeTermRaw(nullptr, L"c"));

				auto b = st.pat[1].get();
				b->children.push_back(makeTermRaw(b, L"b1"));
				b->children.push_back(makeTermRaw(b, L"b3"));
				b->children.push_back(makeTermRaw(b, L"b3"));
			}

			st.expRes = false;

			cases2 += std::move(st);
		}
	}
}