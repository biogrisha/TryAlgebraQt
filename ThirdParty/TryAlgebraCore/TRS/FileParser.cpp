#include "FileParser.h"
#include "TokenMatcher.h"
#include <Me/include/MeCharacter.h>

namespace TryAlgebraCore::Trs
{
	void FileParser::parse(const std::wstring& string, const ParsingRules& rules)
	{
		m_str = string;
		TokenMatcher matcher(rules.tokens);

		bool inv = false;
		while (m_chPos < string.size())
		{
			if (auto match = matcher.findNext(string, m_chPos))
			{
				const std::wstring& token = rules.tokens[match->tokenIndex];
				auto from = m_chPos;
				if (rules.identitySections.contains(token))
				{
					if (waitToken(token))
					{
						const auto substr = std::wstring_view{ string }.substr(from, m_chPos - token.size() - from);
						auto identities = parseIdentities(substr);
						if (!identities.empty())
						{
							auto& section = m_sections.emplace_back();
							section.sectionName = token;
							section.identities = std::move(identities);
							section.hasIdentities = true;
						}
					}
					else
					{
						return;
					}
				}
				else
				{
					auto& newSection = m_sections.emplace_back();
					newSection.sectionName = token;
				}
			}
		}
	}

	bool FileParser::waitToken(const std::wstring& token)
	{
		int progress = 0;

		while (m_chPos < m_str.size())
		{
			wchar_t ch = m_str[m_chPos];
			m_chPos++;
			if (ch == token[progress])
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

	std::vector<std::vector<std::unique_ptr<TermIntermediate>>> FileParser::parseIdentities(const std::wstring_view& str)
	{
		std::vector<std::vector<std::unique_ptr<TermIntermediate>>> identities;
		std::vector<IdentityIntermediate> res;
		auto terms = parseToTermIntermediate(str);
		identities.emplace_back();

		for (auto& t : terms)
		{
			if (t->label == L"=")
			{
				if (!identities.back().empty())
				{
					identities.emplace_back();
				}
				continue;
			}

			identities.back().push_back(std::move(t));
		}
		if (!identities.back().empty())
		{
			identities.pop_back();
		}
		return identities;
	}
}
