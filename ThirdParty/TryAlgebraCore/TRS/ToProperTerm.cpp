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

		std::vector<Identity> trsIdentities;
		for (const auto& id : ids)
		{
			auto& newId = trsIdentities.emplace_back();
			Term* lhsTerm = nullptr;
			toTerm(id.lhs.back(), newId.t_lhs);
			toTerm(id.rhs.back(), newId.t_rhs);
		}
		Term* trsLhs = nullptr;
		Term* trsRhs = nullptr;
		toTerm(m_terms.back(), trsLhs);
		toTerm(equation.rhs.back(), trsRhs);

		Trs trs;
		trs.func(trsIdentities, trsLhs, trsRhs);
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
							, token == tokens.back() ? RuleType::SimpleRecursive : RuleType::RecursiveExhausting);
					}
				}
			}
		}
		m_trsIdentitiesParser.refine(m_transformer);
	}

	void ToProperTerm::toTerm(const std::unique_ptr<TermIntermediate>& from, Term*& to, Term* parent)
	{
		to = new Term;
		to->variable = from->isVariable;
		to->label = from->label;
		to->e_rep = to;
		to->e_reps.push_back(to);
		if (parent)
		{
			to->parents.insert(parent);
		}
		for (auto& ch : from->children)
		{
			Term*& childTerm = to->children.emplace_back(nullptr);
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
