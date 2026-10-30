#pragma once

#include "TestHelpers.h"
#include "TestGlobals.h"
#include <iostream>
#include <span>
#include <unordered_map>
#include <functional>
#include <TRS/PatternMatchingHelpers.h>

namespace PatternMatchingTest
{
	using namespace TryAlgebraCore::Trs;
	class Parser
	{
	public:

		Parser(const std::wstring& str)
			: m_str(str)
		{

		}

		std::vector<std::unique_ptr<TermIntermediate>> parse()
		{
			m_pos = 0;
			return parseTerms(nullptr);
		}

	private:
		std::vector<std::unique_ptr<TermIntermediate>> parseTerms(TermIntermediate* parent)
		{
			std::vector<std::unique_ptr<TermIntermediate>> terms;
			while (m_pos < m_str.size() && m_str[m_pos] != L')')
			{
				const auto termStart = m_pos;
				consumeTermName();

				auto term = std::make_unique<TermIntermediate>();
				term->label = m_str.substr(termStart, m_pos - termStart);
				term->parent = parent;

				if (m_pos < m_str.size() && m_str[m_pos] == L'(')
				{
					++m_pos;
					term->children = parseTerms(term.get());
					if (m_pos < m_str.size() && m_str[m_pos] == L')')
					{
						++m_pos;
					}
				}

				terms.push_back(std::move(term));
				if (m_pos < m_str.size() && m_str[m_pos] == L',')
				{
					++m_pos;
				}
			}
			return terms;
		}

		void consumeTermName()
		{
			auto i = m_pos;
			for (; i < m_str.size(); ++i)
			{
				if (m_str[i] == L'(' || m_str[i] == L')' || m_str[i] == L',')
				{
					break;
				}
			}
			m_pos = i;
		}

		std::wstring m_str;
		std::wstring::size_type m_pos = 0;
	};

	void appendTermsToString(
		const std::vector<std::unique_ptr<TermIntermediate>>& terms,
		std::wstring& result)
	{
		for (std::size_t i = 0; i < terms.size(); ++i)
		{
			const auto& term = terms[i];
			result += term->label;
			if (!term->children.empty())
			{
				result += L'(';
				appendTermsToString(term->children, result);
				result += L')';
			}
			if (i + 1 < terms.size())
			{
				result += L',';
			}
		}
	}

	std::wstring termsToString(const std::vector<std::unique_ptr<TermIntermediate>>& terms)
	{
		std::wstring result;
		appendTermsToString(terms, result);
		return result;
	}

	void markVariables(std::vector<std::unique_ptr<TermIntermediate>>& terms)
	{
		for (auto& term : terms)
		{
			if (term->label.empty())
			{
				continue;
			}
			if (term->label[0] == L'`')
			{
				term->isVariable = true;
				term->variableMeta = std::make_shared<VariableMeta>();
				term->variableMeta->type = VariableMeta::Type::OneMulti;
			}
			else if (term->label[0] == L'~')
			{
				term->isVariable = true;
				term->variableMeta = std::make_shared<VariableMeta>();
				term->variableMeta->type = VariableMeta::Type::Uni;
			}
			else if (term->label[0] == L'_')
			{
				term->isVariable = true;
				term->variableMeta = std::make_shared<VariableMeta>();
				term->variableMeta->type = VariableMeta::Type::ZeroMulti;
			}
			else
			{
				PatternMatchingTest::markVariables(term->children);
			}
		}
	}


	void print(const std::span<std::unique_ptr<TermIntermediate>>& terms)
	{
		for (std::size_t i = 0; i < terms.size(); ++i)
		{
			std::wcout << terms[i]->label;
			if (!terms[i]->children.empty())
			{
				std::wcout << L'(';
				print(terms[i]->children);
				std::wcout << L')';
			}
			if (i + 1 < terms.size())
			{
				std::wcout << L',';
			}
		}
	}

	struct ParserCase
	{
		TestFramework::TestData tst;
		std::wstring input;
		std::wstring expected;
	};

	inline void parserTest(const TestFramework::TestData& tst, const ParserCase& tc)
	{
		MY_EQ(termsToString(Parser(tc.input).parse()), tc.expected);
	}

	struct PatternMatchingCase
	{
		TestFramework::TestData tst;
		struct Subject
		{
			std::wstring terms;
			bool expectedMatch = true;
		};
		struct Pair
		{
			std::wstring pat;
			std::vector<Subject> subjects;
		};
		std::vector<Pair> pairs;
		std::vector<std::wstring> inst;
		std::vector<std::wstring> instExpected;
	};

	inline void matchingTest(const TestFramework::TestData& tst, const PatternMatchingCase& tc)
	{
		PatternMatcher pm;
		std::vector<PatternMatcher::Id> instIds;
		std::vector<PatternMatcher::Id> patIds;
		std::vector<std::vector<std::unique_ptr<TermIntermediate>>> subjects;
		for (const auto& pair : tc.pairs)
		{
			auto terms = Parser(pair.pat).parse();
			PatternMatchingTest::markVariables(terms);
			patIds.push_back(pm.addPat(std::move(terms)));
		}
		for (const auto& inst : tc.inst)
		{
			auto terms = Parser(inst).parse();
			PatternMatchingTest::markVariables(terms);
			instIds.push_back(pm.addInst(std::move(terms)));
		}
		for (std::size_t patI = 0; patI < tc.pairs.size(); ++patI)
		{
			for (const auto& subject : tc.pairs[patI].subjects)
			{
				subjects.push_back(Parser(subject.terms).parse());
				MY_EQ(pm.match(patIds[patI], subjects.back()), subject.expectedMatch);
			}
		}

		for (std::size_t i = 0; i < instIds.size(); ++i)
		{
			MY_EQ(termsToString(pm.instantiate(instIds[i])), tc.instExpected[i]);
		}
	}

	MYTEST(PatternMatchingTest)
	{
		TestFramework::Cases<ParserCase, TestFramework::CasesBehavior::All> parserCases(
			{
				{{"PatternMatchingParserTest", "empty sequence"}, L"", L""},
				{{"PatternMatchingParserTest", "single leaf"}, L"term", L"term"},
				{{"PatternMatchingParserTest", "single root"}, L"t(a,b)", L"t(a,b)"},
				{{"PatternMatchingParserTest", "leaf sequence"}, L"a,b,c,d", L"a,b,c,d"},
				{{"PatternMatchingParserTest", "term sequence"}, L"t(a),t2(b,c),t3", L"t(a),t2(b,c),t3"},
				{{"PatternMatchingParserTest", "nested sequence"}, L"t(a(b,c),d),t2,t3(e(f),g)", L"t(a(b,c),d),t2,t3(e(f),g)"},
				{{"PatternMatchingParserTest", "deep nesting"}, L"a(b(c(d(e))))", L"a(b(c(d(e))))"},
				{{"PatternMatchingParserTest", "multiple nested branches"}, L"root(left(a,b),middle,right(c,d)),tail", L"root(left(a,b),middle,right(c,d)),tail"},
				{{"PatternMatchingParserTest", "variable labels"}, L"~one,`many,_optional", L"~one,`many,_optional"},
				{{"PatternMatchingParserTest", "multi-character labels"}, L"term1(child11,child12),term2", L"term1(child11,child12),term2"},
			}, &parserTest);

		TestFramework::Cases<PatternMatchingCase, TestFramework::CasesBehavior::All> cases(
			{
				{
					{"PatternMatchingTest", "exact term sequence"},
					{{L"t(a),t2(b,c),t3", {{L"t(a),t2(b,c),t3", true}}}},
					{},
					{}
				},
				{
					{"PatternMatchingTest", "exact leaf"},
					{{L"atom", {{L"atom", true}}}},
					{},
					{}
				},
				{
					{"PatternMatchingTest", "exact nested terms"},
					{{L"root(left(a,b),right(c,d))", {{L"root(left(a,b),right(c,d))", true}}}},
					{},
					{}
				},
				{
					{"PatternMatchingTest", "different leaf does not match"},
					{{L"a,b,c", {{L"a,x,c", false}}}},
					{},
					{}
				},
				{
					{"PatternMatchingTest", "different nested term does not match"},
					{{L"t(a,b)", {{L"t(a,c)", false}}}},
					{},
					{}
				},
				{
					{"PatternMatchingTest", "single variable"},
					{{L"t(a,~x,c)", {{L"t(a,b,c)", true}}}},
					{L"result(~x)"},
					{L"result(b)"}
				},
				{
					{"PatternMatchingTest", "top-level single variable"},
					{{L"a,~x,c", {{L"a,b,c", true}}}},
					{L"~x,copy(~x)"},
					{L"b,copy(b)"}
				},
				{
					{"PatternMatchingTest", "single variable rejects multiple terms"},
					{{L"a,~x,c", {{L"a,b,d,c", false}}}},
					{},
					{}
				},
				{
					{"PatternMatchingTest", "one-or-more variable"},
					{{L"a,`x,c", {{L"a,b,d,c", true}}}},
					{L"result(`x)"},
					{L"result(b,d)"}
				},
				{
					{"PatternMatchingTest", "nested one-or-more variable"},
					{{L"t(a,`x,c)", {{L"t(a,b,d,c)", true}}}},
					{L"wrapper(`x)"},
					{L"wrapper(b,d)"}
				},
				{
					{"PatternMatchingTest", "empty zero-or-more variable"},
					{{L"a,_x,c", {{L"a,c", true}}}},
					{L"result(_x)"},
					{L"result"}
				},
				{
					{"PatternMatchingTest", "non-empty zero-or-more variable"},
					{{L"a,_x,c", {{L"a,b,d,c", true}}}},
					{L"result(_x)"},
					{L"result(b,d)"}
				},
				{
					{"PatternMatchingTest", "repeated variable"},
					{{L"~x,middle,~x", {{L"value,middle,value", true}}}},
					{L"pair(~x,~x)"},
					{L"pair(value,value)"}
				},
				{
					{"PatternMatchingTest", "multiple patterns"},
					{
						{L"first(a)", {{L"first(a)", true}}},
						{L"second(b,c),tail", {{L"second(b,c),tail", true}}}
					},
					{},
					{}
				},
			}, &matchingTest);
		{
			PatternMatchingCase cs;
			cs.tst = { "PatternMatchingTest", "1" };

			{
				PatternMatchingCase::Pair pr;
				pr.pat = L"_a,int(cont(`x,=,`from))";
				pr.subjects = {
					{L"ds,c", false},
					{L"a,b,c,int(cont(`x,=,`from)),d", false},
					{L"a,b,c,d,int(cont(f,r,o,m,=,1,0))", true},
				};
				cs.pairs.push_back(std::move(pr));
			}
			cs.inst =
			{
				L"_a",
				L"`x",
				L"`from",
			};
			cs.instExpected =
			{
				L"a,b,c,d",
				L"f,r,o,m",
				L"1,0"
			};
			cases += std::move(cs);
		}


		{
			PatternMatchingCase cs;
			cs.tst = { "PatternMatchingTest", "2" };

			{
				PatternMatchingCase::Pair pr;
				pr.pat = L"_a,int(cont(`x,=,`x))";
				pr.subjects = {
					{L"ds,c", false},
					{L"a,b,c,int(cont(`x,=,`from)),d", false},
					{L"a,b,c,d,int(cont(f,r,o,m,=,1,0))", false},
					{L"a1,a2,a3,int(cont(x1,x2,x3,x4,=,x1,x2,x3,x4))", true},
				};
				cs.pairs.push_back(std::move(pr));
			}

			{
				PatternMatchingCase::Pair pr;
				pr.pat = L"_a,int(cont(`x,=,`x)),`y";
				pr.subjects = {
					{L"ds,c", false},
					{L"a,b,c,int(cont(`x,=,`from)),d", false},
					{L"a,b,c,d,int(cont(f,r,o,m,=,1,0))", false},
					{L"a1,a2,a3,int(cont(x1,x2,x3,x4,=,x1,x2,x3,x4)),y1,y2,y3,y4", true},
				};
				cs.pairs.push_back(std::move(pr));
			}
			cs.inst =
			{
				L"_a",
				L"`x",
				L"`y",
			};
			cs.instExpected =
			{
				L"a1,a2,a3",
				L"x1,x2,x3,x4",
				L"y1,y2,y3,y4"
			};
			cases += std::move(cs);
		}

		{
			PatternMatchingCase cs;
			cs.tst = { "PatternMatchingTest", "second pattern uses single-variable capture" };

			{
				PatternMatchingCase::Pair pr;
				pr.pat = L"capture(~x)";
				pr.subjects = {
					{L"other(first)", false},
					{L"capture(second)", true},
				};
				cs.pairs.push_back(std::move(pr));
			}
			{
				PatternMatchingCase::Pair pr;
				pr.pat = L"use(~x)";
				pr.subjects = {
					{L"use(first)", false},
					{L"use(second)", true},
				};
				cs.pairs.push_back(std::move(pr));
			}

			cs.inst = { L"~x" };
			cs.instExpected = { L"second" };
			cases += std::move(cs);
		}

		{
			PatternMatchingCase cs;
			cs.tst = { "PatternMatchingTest", "second pattern uses resolved term sequence" };

			{
				PatternMatchingCase::Pair pr;
				pr.pat = L"begin,`items,end";
				pr.subjects = {
					{L"begin,a,b,end", true},
				};
				cs.pairs.push_back(std::move(pr));
			}
			{
				PatternMatchingCase::Pair pr;
				pr.pat = L"prefix,`items,suffix";
				pr.subjects = {
					{L"prefix,a,suffix", false},
					{L"prefix,a,b,suffix", true},
				};
				cs.pairs.push_back(std::move(pr));
			}

			cs.inst = { L"list(`items)" };
			cs.instExpected = { L"list(a,b)" };
			cases += std::move(cs);
		}

		{
			PatternMatchingCase cs;
			cs.tst = { "PatternMatchingTest", "second pattern depends on two captured variables" };

			{
				PatternMatchingCase::Pair pr;
				pr.pat = L"record(~key,sep,`values)";
				pr.subjects = {
					{L"record(id,sep,v1,v2)", true},
				};
				cs.pairs.push_back(std::move(pr));
			}
			{
				PatternMatchingCase::Pair pr;
				pr.pat = L"lookup(~key,arrow,`values)";
				pr.subjects = {
					{L"lookup(other,arrow,v1,v2)", false},
					{L"lookup(id,arrow,v1,v2)", true},
				};
				cs.pairs.push_back(std::move(pr));
			}

			cs.inst = { L"result(~key,`values)" };
			cs.instExpected = { L"result(id,v1,v2)" };
			cases += std::move(cs);
		}

		{
			PatternMatchingCase cs;
			cs.tst = { "PatternMatchingTest", "three-pattern dependency chain" };

			{
				PatternMatchingCase::Pair pr;
				pr.pat = L"source(~x)";
				pr.subjects = {
					{L"source(alpha)", true},
				};
				cs.pairs.push_back(std::move(pr));
			}
			{
				PatternMatchingCase::Pair pr;
				pr.pat = L"pair(~x,~y)";
				pr.subjects = {
					{L"pair(wrong,beta)", false},
					{L"pair(alpha,beta)", true},
				};
				cs.pairs.push_back(std::move(pr));
			}
			{
				PatternMatchingCase::Pair pr;
				pr.pat = L"result(~y,~x)";
				pr.subjects = {
					{L"result(alpha,beta)", false},
					{L"result(beta,alpha)", true},
				};
				cs.pairs.push_back(std::move(pr));
			}

			cs.inst = { L"final(~x,~y)" };
			cs.instExpected = { L"final(alpha,beta)" };
			cases += std::move(cs);
		}
	}
}
