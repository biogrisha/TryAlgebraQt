#include "ToProperTerm.h"
#include <vector>
#include "TokenMatcher.h"
#include <iostream>

namespace TryAlgebraCore::Trs
{
	namespace Tokens
	{
		const std::wstring tdEx = L"-tdEx";
		const std::wstring tdRec = L"-tdRec";
		const std::wstring buEx = L"-buEx";
		const std::wstring buRec = L"-buRec";
		const std::wstring rules = L"-rules";
		const std::wstring inv = L"-inv";
	}
	std::wstring ToProperTerm::run(const std::wstring& string)
	{
		IdentityIntermediate equation = std::move(parseIdentities(string).back());
		m_terms = std::move(equation.lhs);
		markVariables(m_terms);
		m_transformer.applyAll(m_terms);
		m_transformer.applyAll(equation.rhs);
		/*if (true)
		{
			return L"";
		}*/
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
		auto res = trs.run(id, trsIdentities);

		/*std::wstring resStr;
		for (auto& subs : res)
		{
			resStr += L"\n=======";
			for (auto& [var, subj] : subs)
			{
				resStr += L"\n";
				{
					std::vector<std::unique_ptr<TermIntermediate>> lhs;
					lhs.emplace_back();
					toIntermediate(var, lhs.back());

					std::wstring str;
					m_transformer.applyAllInverse(lhs);
					m_terms = std::move(lhs);
					termIntermediateToStr(m_terms, str);
					resStr += str + L"=";
				}

				{
					std::vector<std::unique_ptr<TermIntermediate>> rhs;
					rhs.emplace_back();
					toIntermediate(subj, rhs.back());


					m_transformer.applyAllInverse(rhs);
					m_terms = std::move(rhs);
					std::wstring str;
					termIntermediateToStr(m_terms, str);
					resStr += str;
				}
			}
		}
		return resStr;*/
		return L"";
	}

	const std::vector<std::unique_ptr<TermIntermediate>>& ToProperTerm::get() const
	{
		return m_terms;
	}

	void ToProperTerm::setup(const TextBuffer& tb)
	{
		std::vector<std::wstring> tokens = {
			Tokens::buEx,
			Tokens::buRec,
			Tokens::inv,
			Tokens::rules,
			Tokens::tdEx,
			Tokens::tdRec,
		};


		TokenMatcher matcher(tokens);

		TextBufferIterator it(tb, 0);

		bool inv = false;
		while (!it.isEnd())
		{
			if (auto match = matcher.findNext(it))
			{
				const std::wstring& token = tokens[match->tokenIndex];
				auto from = it.getChId();
				if (token == Tokens::inv)
				{
					inv = true;
					continue;
				}
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
							, tokenToType(token), inv);
					}
				}
			}
		}
		m_trsIdentitiesParser.refine(m_transformer);
	}

	RuleType ToProperTerm::tokenToType(const std::wstring& token) const
	{
		if (token == Tokens::buEx)
		{
			return RuleType::BURecursiveExhausting;
		}
		if (token == Tokens::buRec)
		{
			return RuleType::BUSimpleRecursive;
		}
		if (token == Tokens::tdEx)
		{
			return RuleType::TDRecursiveExhausting;
		}
		if (token == Tokens::tdRec)
		{
			return RuleType::TDSimpleRecursive;
		}
		return RuleType::None;
	}

	void ToProperTerm::toTerm(const std::unique_ptr<TermIntermediate>& from, NewTrs::Term*& to, NewTrs::Term* parent)
	{
		to = new NewTrs::Term;
		to->isVariable = from->isVariable;
		//const auto& [it, inserted] = m_symbols.emplace(from->label, m_ch);
		//if (inserted)
		//{
		//	m_ch++;
		//	it->second = m_ch;
		//	m_symbolsInv[m_ch] = from->label;
		//}
		//to->label = std::string(1, it->second);
		to->label = std::string(from->label.begin(), from->label.end());
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

	void ToProperTerm::toIntermediate(NewTrs::Term* term, std::unique_ptr<TermIntermediate>& intermediate)
	{
		intermediate = std::make_unique<TermIntermediate>();
		//intermediate->label = m_symbolsInv[term->label.back()];
		intermediate->label = std::wstring(term->label.begin(), term->label.end());
		for (NewTrs::Term* ch : term->children)
		{
			auto& newCh = intermediate->children.emplace_back();
			toIntermediate(ch, newCh);
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

	void ToProperTerm::print(const std::vector<std::unique_ptr<TermIntermediate>>& terms)
	{
		if (terms.empty())
		{
			return;
		}
		std::wcout << "(";
		for (auto& t : terms)
		{
			std::wcout << t->label;
			print(t->children);
			if (t != terms.back())
			{
				std::wcout << ",";
			}
		}
		std::wcout << ")";
	}

}
