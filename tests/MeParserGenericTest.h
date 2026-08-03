#pragma once
#include "TestHelpers.h"
#include "TestGlobals.h"
#include "TestFramework.h"
#include <TRS/MeParserGeneric.h>
namespace MeParserGenericTest
{
	using namespace TryAlgebraCore;
	struct State
	{
		std::wstring str;
		int i = 0;
		bool operator ==(const State& other) const
		{
			return str == other.str && i == other.i;
		}
	};

	inline std::wostream& operator<<(std::wostream& os, const std::vector<State>& states)
	{
		os << L"[";

		bool first = true;

		for (const auto& st : states)
		{
			if (!first)
				os << L",";
			os << st.str << " " << st.i;
			first = false;
		}

		os << "]";
		return os;
	}
	struct MeParserGenericCase
	{
		TestFramework::TestData tst;
		std::wstring str;
		std::vector<State> res;
	};

	inline void createMe(const std::wstring_view& str, std::vector<State>& state)
	{
		state.emplace_back(std::wstring(str), 1);
	}
	inline void addMeta(const std::wstring_view& str, std::vector<State>& state)
	{
		state.emplace_back(std::wstring(str), 2);
	}
	inline void addGlyph(wchar_t g, std::vector<State>& state)
	{
		state.emplace_back(std::wstring(1, g), 3);
	}
	inline void startChildren(std::vector<State>& state)
	{
		state.emplace_back(L"", 4);
	}
	inline void endChildren(std::vector<State>& state)
	{
		state.emplace_back(L"", 5);
	}
	inline void nextChild(std::vector<State>& state)
	{
		state.emplace_back(L"", 6);
	}

	inline void addGlyphs(const std::wstring& str, std::vector<State>& state)
	{
		for (auto ch : str)
		{
			addGlyph(ch, state);
		}
	}
	inline void addMe(const std::wstring& meName, const std::wstring& meta, std::vector<State>& state, bool hasChildren = true)
	{
		createMe(meName, state);
		addMeta(meta, state);
		if (hasChildren)
			startChildren(state);
	}
	inline void addMe(const std::wstring& meName, std::vector<State>& state, bool hasChildren = true)
	{
		createMe(meName, state);
		if (hasChildren)
			startChildren(state);
	}

	inline void parseTest(const TestFramework::TestData& tst, const MeParserGenericCase& tc)
	{
		Trs::MeParserGeneric parser(tc.str);
		std::vector<State> res;
		parser.createMe = [&res](const std::wstring_view& str) {createMe(str, res); };
		parser.addMeta = [&res](const std::wstring_view& str) {addMeta(str, res); };
		parser.addGlyph = [&res](wchar_t g) {addGlyph(g, res); };
		parser.startChildren = [&res]() {startChildren(res); };
		parser.endChildren = [&res]() {endChildren(res); };
		parser.nextChild = [&res]() {nextChild(res); };
		parser.parse();
		MY_EQ(res, tc.res);
	}

	MYTEST(MeParserGenericTest)
	{
		TestFramework::Cases<MeParserGenericCase, TestFramework::CasesBehavior::All> cases(
			{
			}, &parseTest);
		{
			std::vector<State> res;
			addGlyphs(L"abc", res);
			cases += MeParserGenericCase({ "MeParserGenericTest", "1" }, L"abc", res);
		}
		{
			std::vector<State> res;
			addGlyphs(L"abc\nefg", res);
			cases += MeParserGenericCase({ "MeParserGenericTest", "2" }, L"abc\nefg", res);
		}
		{
			std::vector<State> res;
			addGlyphs(L"y5yr", res);
			addMe(L"ft", L"1", res);
			nextChild(res);
			addMe(L"ft", L"1", res);
			addGlyphs(L"yr5y5r", res);
			nextChild(res);
			addGlyphs(L"y5ryr5", res);
			endChildren(res);
			endChildren(res);
			addGlyphs(L"yr5y", res);
			addMe(L"ft", L"2", res);
			nextChild(res);
			addGlyphs(L"yr5", res);
			endChildren(res);
			addGlyphs(L"yr5", res);
			cases += MeParserGenericCase({ "MeParserGenericTest", "3" }, L"y5yr\\ft\\1\\{\\,\\ft\\1\\{yr5y5r\\,y5ryr5\\}\\}yr5y\\ft\\2\\{\\,yr5\\}yr5", res);
		}
		{
			std::vector<State> res;
			addGlyphs(L"ab", res);
			addMe(L"func1", L"meta1", res);
			nextChild(res);
			endChildren(res);
			addGlyphs(L"cd", res);
			cases += MeParserGenericCase({ "MeParserGenericTest", "4" }, L"ab\\func1\\meta1\\{\\,\\}cd", res);
		}
		{
			std::vector<State> res;
			addGlyphs(L"ab", res);
			addMe(L"func1", res);
			nextChild(res);
			endChildren(res);
			addGlyphs(L"cd", res);
			cases += MeParserGenericCase({ "MeParserGenericTest", "5" }, L"ab\\func1\\{\\,\\}cd", res);
		}
		{
			std::vector<State> res;
			addGlyphs(L"ab", res);
			addMe(L"func1", L"meta1", res, false);
			addGlyphs(L"cd", res);
			cases += MeParserGenericCase({ "MeParserGenericTest", "6" }, L"ab\\func1\\meta1\\cd", res);
		}
		{
			std::vector<State> res;
			addGlyphs(L"ab", res);
			addMe(L"func1", L"meta1", res);
			endChildren(res);
			addGlyphs(L"cd", res);
			cases += MeParserGenericCase({ "MeParserGenericTest", "7" }, L"ab\\func1\\meta1\\{\\}cd", res);
		}
		{
			std::vector<State> res;
			addGlyphs(L"ab", res);
			addMe(L"func1", res);
			nextChild(res);
			nextChild(res);
			nextChild(res);
			endChildren(res);
			addGlyphs(L"cd", res);
			cases += MeParserGenericCase({ "MeParserGenericTest", "8" }, L"ab\\func1\\{\\,\\,\\,\\}cd", res);
		}
		{
			std::vector<State> res;
			addGlyphs(L"ab", res);
			addMe(L"func1", res);
			nextChild(res);
			addGlyphs(L"ab", res);
			addMe(L"func2", L"meta2", res, false);
			addMe(L"func3", L"meta3", res);
			addGlyphs(L"ggg", res);
			nextChild(res);
			addGlyphs(L"gg", res);
			endChildren(res);
			nextChild(res);
			nextChild(res);
			endChildren(res);
			addGlyphs(L"cd", res);
			cases += MeParserGenericCase({ "MeParserGenericTest", "9" }, L"ab\\func1\\{\\,ab\\func2\\meta2\\\\func3\\meta3\\{ggg\\,gg\\}\\,\\,\\}cd", res);
		}
		{
			std::vector<State> res;
			addGlyphs(L"ab", res);
			addMe(L"func1", L"meta1", res);
			endChildren(res);
			cases += MeParserGenericCase({ "MeParserGenericTest", "10" }, L"ab\\func1\\meta1\\{\\}", res);
		}
	}

}
