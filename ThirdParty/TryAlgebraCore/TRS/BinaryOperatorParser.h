#pragma once
#include "PatternMatchingHelpers.h"
#include "BracketsParser.h"

namespace TryAlgebraCore::Trs
{
	enum class RuleType
	{
		TDSimpleRecursive,
		TDRecursiveExhausting,
		BUSimpleRecursive
	};

	struct RewritingRule
	{
		std::vector<std::unique_ptr<TermIntermediate>> from;
		std::vector<std::unique_ptr<TermIntermediate>> to;
		std::vector<TermIntermediate*> variables;
		std::vector<Level> levels;
		std::vector<Bundle*> bundles;
		RuleType type;
	};

	class Transformer
	{
	public:
		void applyAll(std::vector<std::unique_ptr<TermIntermediate>>& subj);
		void applyAllInverse(std::vector<std::unique_ptr<TermIntermediate>>& subj);

		void tdSimpleRecursive(std::vector<std::unique_ptr<TermIntermediate>>& subj, RewritingRule& rule);
		void tdRecursiveExhausting(std::vector<std::unique_ptr<TermIntermediate>>& subj, RewritingRule& rule);

		void addRules(const std::wstring& rawStr, RuleType type);
		void removeContainers(std::vector<std::unique_ptr<TermIntermediate>>& subj);
		void addContainers(std::vector<std::unique_ptr<TermIntermediate>>& subj);
	private:
		bool isTrm(const std::wstring& label) const;
		std::vector<RewritingRule> m_rules;
		BracketsParser bracketsParser;
	};
}