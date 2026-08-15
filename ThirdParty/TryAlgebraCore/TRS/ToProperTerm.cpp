#include "ToProperTerm.h"
#include <vector>
#include "TokenMatcher.h"

namespace TryAlgebraCore::Trs
{
	void ToProperTerm::run(const std::wstring& string)
	{
		IdentityIntermediate equation = std::move(parseIdentities(string).back());
		m_terms = std::move(equation.lhs);
		markVariables(m_terms);
		m_transformer.applyAll(m_terms);
		m_transformer.applyAll(equation.rhs);

		const auto& ids = m_trsIdentitiesParser.identities();

		std::vector<NewTrs::Identity> trsIdentities;
		for (const auto& id : ids)
		{
			auto& newId = trsIdentities.emplace_back();
			NewTrs::Term* lhsTerm = nullptr;
			toTerm(id.lhs.back(), newId.lhs);
			toTerm(id.rhs.back(), newId.rhs);
		}
		NewTrs::Identity id;
		id.lhs = nullptr;
		id.rhs = nullptr;
		toTerm(m_terms.back(), id.lhs);
		toTerm(equation.rhs.back(), id.rhs);

		NewTrs::Trs trs;
		trs.run(id, trsIdentities);
	}

	const std::vector<std::unique_ptr<TermIntermediate>>& ToProperTerm::get() const
	{
		return m_terms;
	}

	void ToProperTerm::setup(const TextBuffer& tb)
	{
		std::vector<std::wstring> tokens = {
			L"-ex",
			L"-rec",
			L"-rules"
		};

		TokenMatcher matcher(tokens);

		TextBufferIterator it(tb, 0);

		while (!it.isEnd())
		{
			if (auto match = matcher.findNext(it))
			{
				const std::wstring& token = tokens[match->tokenIndex];
				auto from = it.getChId();
				if (waitToken(it, token))
				{
					auto to = it.getChId() - token.size();
					if (token == L"-rules")
					{
						m_trsIdentitiesParser.setup(tb.getSubstring(from, to));
					}
					else
					{
						m_transformer.addRules(tb.getSubstring(from, to)
							, token == L"-rec" ? RuleType::SimpleRecursive : RuleType::RecursiveExhausting);
					}
				}
			}
		}
		m_trsIdentitiesParser.refine(m_transformer);
	}

	void ToProperTerm::toTerm(const std::unique_ptr<TermIntermediate>& from, NewTrs::Term*& to, NewTrs::Term* parent)
	{
		to = new NewTrs::Term;
		to->isVariable = from->isVariable;
		const auto& [it, inserted] = m_symbols.emplace(from->label, m_ch);
		if (inserted)
		{
			m_ch++;
			it->second = m_ch;
		}
		to->label = std::string(1, it->second);
		to->eRep = to;
		to->eReps.push_back(to);
		if (parent)
		{
			to->parents.insert(parent);
		}
		for (auto& ch : from->children)
		{
			NewTrs::Term*& childTerm = to->children.emplace_back(nullptr);
			toTerm(ch, childTerm, to);
		}
	}

	std::optional<std::wstring> ToProperTerm::getSection(const TextBuffer& tb, const std::wstring& sectionName)
	{
		TextBufferIterator it(tb, 0);

		if (!waitToken(it, sectionName))
		{
			return std::nullopt;
		}
		int start = it.getChId();
		if (!waitToken(it, sectionName))
		{
			return std::nullopt;
		}

		return tb.getSubstring(start, it.getChId() - sectionName.size());
	}

	bool ToProperTerm::waitToken(TextBufferIterator& it, const std::wstring& token)
	{
		int progress = 0;

		while (!it.isEnd())
		{
			if (it.next() == token[progress])
			{
				++progress;
				if (progress == token.size())
				{
					break;
				}
			}
			else
			{
				progress = 0;
			}
		}
		return progress = token.size();
	}

}
