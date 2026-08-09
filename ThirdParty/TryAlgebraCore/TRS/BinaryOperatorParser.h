#pragma once
#include "PatternMatchingHelpers.h"

namespace TryAlgebraCore::Trs
{
	enum class RuleType
	{
		SimpleRecursive,
		RecursiveExhausting
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

	class BinaryOperatorParser
	{
	public:
		void applyAll(std::vector<std::unique_ptr<TermIntermediate>>& subj);
		void simpleRecursive(std::vector<std::unique_ptr<TermIntermediate>>& subj, RewritingRule& rule);
		void recursiveExhausting(std::vector<std::unique_ptr<TermIntermediate>>& subj, RewritingRule& rule);
		void addRules(const std::wstring& rawStr, RuleType type);
	private:
		std::vector<RewritingRule> m_rules;
	};
}