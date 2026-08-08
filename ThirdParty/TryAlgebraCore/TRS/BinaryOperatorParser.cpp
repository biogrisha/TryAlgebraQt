#include "BinaryOperatorParser.h"
#include <Me/include/MeGlobals.h>

namespace TryAlgebraCore::Trs
{
	void BinaryOperatorParser::applyAll(std::vector<std::unique_ptr<TermIntermediate>>& subj)
	{
		for (auto& rule : m_rules)
		{
			apply(subj, rule);
		}
	}
	void BinaryOperatorParser::apply(std::vector<std::unique_ptr<TermIntermediate>>& subj, RewritingRule& rule)
	{
		if (subj.empty())
		{
			return;
		}
		if (subj.back()->label != MeNames::cont)
		{

			if (tryMatch(rule.bundles, rule.from, subj))
			{
				std::vector<std::unique_ptr<TermIntermediate>> tmp;
				copyTermIntermediate(rule.to, tmp);
				expandVars(tmp);
				subj = std::move(tmp);
			}
			for (auto* var : rule.variables)
			{
				var->variableMeta->captured = {};
			}
		}
		for (auto& t : subj)
		{
			apply(t->children, rule);
		}
	}

	void BinaryOperatorParser::addRules(const std::wstring& rawStr)
	{
		auto terms = parseToTermIntermediate(rawStr);
		int i = 0;
		while (i < terms.size())
		{
			RewritingRule rule;
			for (; i < terms.size(); ++i)
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
				rule.from.push_back(std::move(terms[i]));
			}
			for (; i < terms.size(); ++i)
			{
				if (terms[i]->label == L" ")
				{
					continue;
				}
				if (terms[i]->label == L"\n")
				{
					break;
				}

				rule.to.push_back(std::move(terms[i]));
			}
			markVariables(rule.from);
			markVariables(rule.to);
			unifyVariables(rule.from, rule.variables);
			unifyVariables(rule.to, rule.variables);
			markPatternNodes(rule.from);
			rule.levels = generateLevels(rule.from);
			for (auto& b : rule.levels.front().bundles)
			{
				rule.bundles.push_back(&b);
			}
			m_rules.push_back(std::move(rule));
		}
	}
}
