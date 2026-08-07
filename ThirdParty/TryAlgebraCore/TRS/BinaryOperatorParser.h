#pragma once
#include "PatternMatchingHelpers.h"

namespace TryAlgebraCore::Trs
{
	struct RewritingRule
	{
		std::vector<std::unique_ptr<TermIntermediate>> from;
		std::vector<std::unique_ptr<TermIntermediate>> to;
		std::vector<TermIntermediate*> variables;
		std::vector<Level> levels;
		std::vector<Bundle*> bundles;
	};
	class BinaryOperatorParser
	{
	public:
		void apply(std::vector<std::unique_ptr<TermIntermediate>>& subj);
		void addRules(const std::wstring& rawStr);
	private:
		RewritingRule m_rule;
	};
}