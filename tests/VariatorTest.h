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

	struct VariatorTestCase
	{
		TestFramework::TestData tst;
		int sum = 0;
		std::vector<int> offsets;
		std::vector<int> offsetsRes;
	};

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

	// ============================================================
	// Helpers
	// ============================================================

	inline TermRawSh makeTermRaw(
		TermRaw* parent,
		const std::wstring& label,
		bool variable = false)
	{
		auto res = std::make_shared<TermRaw>();
		res->label = label;
		res->variable = variable;
		res->parent = parent;
		return res;
	}

	inline std::vector<TermRawSh> makeTerms(
		std::initializer_list<std::pair<std::wstring, bool>> items)
	{
		std::vector<TermRawSh> result;

		for (const auto& [label, variable] : items)
			result.push_back(makeTermRaw(nullptr, label, variable));

		return result;
	}

	inline void addChildren(
		TermRawSh& parent,
		std::initializer_list<std::wstring> children)
	{
		auto* p = parent.get();

		for (const auto& label : children)
			p->children.push_back(makeTermRaw(p, label));
	}

	inline OffsetsToSizesCase makeOffsetsToSizesCase(
		const char* caseName,
		int sum,
		std::vector<int> offsets,
		std::vector<int> sizes)
	{
		OffsetsToSizesCase c;
		c.tst = { "OffsetsToSize", caseName };
		c.sum = sum;
		c.offsets = std::move(offsets);
		c.sizes = std::move(sizes);
		return c;
	}

	inline VariatorTestCase makeVariatorCase(
		const char* caseName,
		int sum,
		std::vector<int> offsets,
		std::vector<int> expected)
	{
		VariatorTestCase c;
		c.tst = { "variatorTest", caseName };
		c.sum = sum;
		c.offsets = std::move(offsets);
		c.offsetsRes = std::move(expected);
		return c;
	}

	inline StateCompareBoundariesCase makeStateCase(
		const char* caseName)
	{
		StateCompareBoundariesCase c;
		c.tst = { "CompBoundaries", caseName };
		return c;
	}

	// ============================================================
	// Tests
	// ============================================================

	inline void offsetsToSizesTest(
		const TestFramework::TestData& tst,
		const OffsetsToSizesCase& data)
	{
		auto res = TryAlgebraCore::Trs::Variator::generateSizes(
			data.offsets,
			data.sum);

		MY_EQ(res, data.sizes);
	}

	inline void variatorTest(
		const TestFramework::TestData& tst,
		const VariatorTestCase& data)
	{
		Variator var(data.offsets.size() + 1, data.sum);

		var.m_offsets = data.offsets;
		var.m_isFirstStep = false;

		var.step();

		MY_EQ(var.m_offsets, data.offsetsRes);
	}

	inline void stateCompareBoundariesTest(
		const TestFramework::TestData& tst,
		const StateCompareBoundariesCase& data)
	{
		auto copy = data;

		PatternMatcher::State state(copy.pat, copy.terms);

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
		TestFramework::Cases<
			OffsetsToSizesCase,
			TestFramework::CasesBehavior::All>
			offsetsToSizesCases({}, &offsetsToSizesTest);

		TestFramework::Cases<
			VariatorTestCase,
			TestFramework::CasesBehavior::All>
			variatorTestCases({}, &variatorTest);

		TestFramework::Cases<
			StateCompareBoundariesCase,
			TestFramework::CasesBehavior::All>
			stateCases({}, &stateCompareBoundariesTest);

		// ========================================================
		// OffsetsToSizes
		// ========================================================

		offsetsToSizesCases += makeOffsetsToSizesCase(
			"case1",
			5,
			{ 1, 2 },
			{ 1, 1, 3 });

		offsetsToSizesCases += makeOffsetsToSizesCase(
			"case2",
			5,
			{ 1, 2, 3 },
			{ 1, 1, 1, 2 });

		offsetsToSizesCases += makeOffsetsToSizesCase(
			"case3",
			6,
			{ 2, 3, 5 },
			{ 2, 1, 2, 1 });

		// ========================================================
		// Variator
		// ========================================================

		variatorTestCases += makeVariatorCase(
			"case1",
			5,
			{ 1, 2, 3 },
			{ 1, 2, 4 });

		variatorTestCases += makeVariatorCase(
			"case2",
			5,
			{ 1, 2, 4 },
			{ 1, 3, 4 });

		variatorTestCases += makeVariatorCase(
			"case3",
			10,
			{ 2, 5, 8, 9 },
			{ 2, 6, 7, 8 });

		variatorTestCases += makeVariatorCase(
			"case4",
			10,
			{ 2, 7, 8, 9 },
			{ 3, 4, 5, 6 });

		variatorTestCases += makeVariatorCase(
			"case5",
			10,
			{ 0, 7, 8, 9 },
			{ 1, 2, 3, 4 });

		variatorTestCases += makeVariatorCase(
			"case6",
			10,
			{ 0, 1 },
			{ 0, 2 });

		variatorTestCases += makeVariatorCase(
			"case7",
			10,
			{ 0, 9 },
			{ 1, 2 });

		// ========================================================
		// StateCompareBoundaries
		// ========================================================

		{
			auto st = makeStateCase("case1");

			st.terms = makeTerms({
				{L"a", false},
				{L"b", false},
				{L"-v1", false},
				{L"-v2", false},
				{L"-v2", false},
				{L"-v2", false},
				{L"-v3", false},
				{L"-v3", false},
				{L"-v3", false},
				{L"c", false}
				});

			st.pat = makeTerms({
				{L"a", false},
				{L"b", false},
				{L"v1", true},
				{L"v2", true},
				{L"v3", true},
				{L"c", false}
				});

			addChildren(st.terms[1], { L"b1", L"b2", L"b3" });
			addChildren(st.pat[1], { L"b1", L"b2", L"b3" });

			st.variablesStartPat = 2;
			st.variablesEndPat = 5;
			st.variablesStartTerms = 2;
			st.variablesEndTerms = 9;

			stateCases += std::move(st);
		}

		// Remaining cases become similarly compact:
		// build terms with makeTerms(...)
		// add children with addChildren(...)
		// set expected values
		// stateCases += std::move(st);
	}
}