#pragma once
#include <span>
#include "Trs.h"
#include <Me/include/MeBase.h>

namespace TryAlgebraCore::Trs
{
	using TermRawSh = std::shared_ptr<struct TermRaw>;
	struct TermRaw
	{
		bool variable = false;
		bool hasVariableChild = false;
		bool pattern = false;
		std::wstring label;
		std::vector<TermRawSh> children;
		std::shared_ptr<std::span<TermRawSh>> captured;
		TermRaw* parent = nullptr;
		TermRaw* patMatch = nullptr;
		int time = 0;
	};

	struct Variator
	{
		Variator(int size, int sum);
		//return true if this step was the last
		bool step();
		bool finished(const std::vector<int>& offsets, const int sum);
		std::vector<int> getSizes();
		static bool isLastPos(const std::vector<int>& offsets, const int sum, const int i);
		static std::vector<int> generateSizes(const std::vector<int>& offsets, const int sum);
		std::vector<int> m_offsets;
		int m_sum = 0;
		bool m_isFirstStep = true;
		bool m_lastStep = false;
	};

	class PatternMatcher
	{
	public:
		struct State
		{
			State(const std::span<TermRawSh>& pat, const std::span<TermRawSh>& terms);
			bool handleVariatorStep();
			bool nextVariation();
			void setVariablesTime();
			bool compBoundaries();
			bool compIntermediate();

			std::span<TermRawSh> terms;
			std::span<TermRawSh> pat;
			std::unique_ptr<Variator> variator;
			
			int variablesStartPat = 0;
			int variablesEndPat = 0;
			int variablesStartTerms = 0;
			int variablesEndTerms = 0;
			int compTermsId = 0;
			int compPatId = 0;
			int time = 0;

			int backId = -1;
			int parentId = -1;
			inline static int staticTime = 0;
		};
		bool match(std::vector<TermRawSh>& pat, const std::span<TermRawSh>& terms);
		bool back();

		std::vector<std::unique_ptr<State>> stack;
	};

	bool hasVariable(const std::vector<TermRawSh>& pat);
	bool compareWithVariable(TermRaw* var, const std::span<TermRawSh>& terms);
	bool topLevelComp(const std::span<TermRawSh>& pat, const std::span<TermRawSh>& terms);
	bool compare(TermRaw* lhs, TermRaw* rhs);
	bool match(const std::span<Term*>& terms, const std::vector<Term*>& pattern, int from);
	void recognizeVariables(std::vector<TermRawSh>& pattern);
	void convertMeToTerms(const std::span<std::unique_ptr<MeBase>>& meList, std::vector<TermRawSh>& result, TermRaw* parent);
	void unifyVariables(std::vector<TermRawSh>& terms, std::vector<TermRaw*>& variables);
	bool compare(const std::span<TermRawSh>& pat, const std::span<TermRawSh>& terms);
}