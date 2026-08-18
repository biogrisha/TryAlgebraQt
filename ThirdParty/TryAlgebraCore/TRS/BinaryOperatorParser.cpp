#include "BinaryOperatorParser.h"
#include <Me/include/MeGlobals.h>

namespace TryAlgebraCore::Trs
{
	void Transformer::applyAll(std::vector<std::unique_ptr<TermIntermediate>>& subj)
	{
		bracketsParser.apply(subj);
		for (auto& rule : m_rules)
		{
			switch (rule.type)
			{
			case RuleType::TDRecursiveExhausting:
				tdRecursiveExhausting(subj, rule);
				break;
			case RuleType::TDSimpleRecursive:
				tdSimpleRecursive(subj, rule);
				break;
			default:
				break;
			}
		}
		bracketsParser.removeBrackets(subj);
		removeContainers(subj);
	}

	void Transformer::applyAllInverse(std::vector<std::unique_ptr<TermIntermediate>>& subj)
	{
		addContainers(subj);
		for (auto& rule : m_invRules)
		{
			switch (rule.type)
			{
			case RuleType::TDRecursiveExhausting:
				tdRecursiveExhausting(subj, rule);
				break;
			case RuleType::TDSimpleRecursive:
				tdSimpleRecursive(subj, rule);
				break;
			default:
				break;
			}
		}
	}

	void Transformer::tdSimpleRecursive(std::vector<std::unique_ptr<TermIntermediate>>& subj, RewritingRule& rule)
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
		if (subj.back()->parent && subj.back()->parent->label == MeNames::term + MeNames::termFunction)
		{
			for (int i = 1; i < subj.size(); ++i)
			{
				tdSimpleRecursive(subj[i]->children, rule);
			}
			return;
		}
		for (auto& t : subj)
		{
			tdSimpleRecursive(t->children, rule);
		}
	}

	void Transformer::tdRecursiveExhausting(std::vector<std::unique_ptr<TermIntermediate>>& subj, RewritingRule& rule)
	{
		if (subj.empty() || subj.back()->label == MeNames::term + MeNames::termToken)
		{
			return;
		}
		if (subj.back()->label != MeNames::cont)
		{
			while (tryMatch(rule.bundles, rule.from, subj))
			{
				std::vector<std::unique_ptr<TermIntermediate>> tmp;
				copyTermIntermediate(rule.to, tmp);
				expandVars(tmp);
				subj = std::move(tmp);
				for (auto* var : rule.variables)
				{
					var->variableMeta->captured = {};
				}
			}
			for (auto* var : rule.variables)
			{
				var->variableMeta->captured = {};
			}
		}
		if (subj.back()->parent && subj.back()->parent->label == MeNames::term + MeNames::termFunction)
		{
			for (int i = 1; i < subj.size(); ++i)
			{
				tdRecursiveExhausting(subj[i]->children, rule);
			}
			return;
		}
		for (auto& t : subj)
		{
			tdRecursiveExhausting(t->children, rule);
		}
	}

	void Transformer::addRules(const std::wstring& rawStr, RuleType type, bool inv)
	{
		auto identities = parseIdentities(rawStr);
		for (auto& id : identities)
		{
			RewritingRule rule;
			rule.from = std::move(id.lhs);
			rule.to = std::move(id.rhs);
			rule.type = type;
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
			if (inv)
			{
				m_invRules.push_back(std::move(rule));
			}
			else
			{
				m_rules.push_back(std::move(rule));
			}
		}
	}

	void Transformer::removeContainers(std::vector<std::unique_ptr<TermIntermediate>>& subj)
	{
		for (int i = 0; i < subj.size(); ++i)
		{
			if (subj[i]->isVariable)
			{
				continue;
			}
			removeContainers(subj[i]->children);
			if (subj[i]->label == MeNames::cont)
			{
				subj[i] = std::move(subj[i]->children.back());
			}
		}
	}
	void Transformer::addContainers(std::vector<std::unique_ptr<TermIntermediate>>& subj)
	{
		for (int i = 0; i < subj.size(); ++i)
		{
			addContainers(subj[i]->children);
			if (isTrm(subj[i]->label))
			{
				auto children = std::move(subj[i]->children);
				subj[i]->children.clear();
				for (auto& ch : children)
				{
					auto& cont = subj[i]->children.emplace_back(std::make_unique<TermIntermediate>());
					cont->label = MeNames::cont;
					cont->children.push_back(std::move(ch));
				}
			}
		}
	}

	bool Transformer::isTrm(const std::wstring& l) const
	{
		return l.size() > 2 && l[0] == L't' && l[1] == L'r' && l[2] == L'm';
	}
}
