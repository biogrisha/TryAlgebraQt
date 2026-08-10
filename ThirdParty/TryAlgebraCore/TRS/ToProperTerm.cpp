#include "ToProperTerm.h"
#include <vector>
#include "TokenMatcher.h"
#include <iostream>

namespace TryAlgebraCore::Trs
{
	void ToProperTerm::run(const std::wstring& string)
	{
		m_terms = parseToTermIntermediate(string);
		m_bracketsParser.apply(m_terms);
		m_binaryOperatorParser.applyAll(m_terms);
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
					m_binaryOperatorParser.addRules(tb.getSubstring(from, to)
						, token == tokens.back() ? RuleType::SimpleRecursive : RuleType::RecursiveExhausting);
				}
			}
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
