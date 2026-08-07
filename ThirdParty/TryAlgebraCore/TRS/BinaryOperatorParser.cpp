#include "BinaryOperatorParser.h"

namespace TryAlgebraCore::Trs
{
	void BinaryOperatorParser::apply(std::vector<std::unique_ptr<TermIntermediate>>& subj)
	{
		if (tryMatch(m_rule.bundles, m_rule.from, subj))
		{
			std::vector<std::unique_ptr<TermIntermediate>> tmp;
			copyTermIntermediate(m_rule.to, tmp);
			expandVars(tmp);
			subj = std::move(tmp);
		}
		for (auto* var : m_rule.variables)
		{
			var->variableMeta->captured = {};
		}
		for (auto& t : subj)
		{
			apply(t->children);
		}

	}

	void BinaryOperatorParser::addRules(const std::wstring& rawStr)
	{
		auto terms = parseToTermIntermediate(rawStr);
		int i = 0;
		for (;i < terms.size(); ++i)
		{
			if (terms[i]->label == L" " || terms[i]->label == L"\n")
			{
				continue;
			}
			if (terms[i]->label == L"=")
			{
				++i;
				break;
			}
			m_rule.from.push_back(std::move(terms[i]));
		}
		for (;i < terms.size(); ++i)
		{
			if (terms[i]->label == L" ")
			{
				continue;
			}
			if (terms[i]->label == L"\n")
			{
				break;
			}

			m_rule.to.push_back(std::move(terms[i]));
		}
		markVariables(m_rule.from);
		markVariables(m_rule.to);
		unifyVariables(m_rule.from, m_rule.variables);
		unifyVariables(m_rule.to, m_rule.variables);
		markPatternNodes(m_rule.from);
		m_rule.levels = generateLevels(m_rule.from);
		for (auto& b : m_rule.levels.front().bundles)
		{
			m_rule.bundles.push_back(&b);
		}
	}
}
