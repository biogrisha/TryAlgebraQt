#include "FileParser.h"
#include "TokenMatcher.h"
#include <Me/include/MeCharacter.h>

namespace TryAlgebraCore::Trs
{
	namespace Tokens
	{

		constexpr const wchar_t Parser = L"$Parser";
		constexpr const wchar_t Formulas = L"$Formulas";
		constexpr const wchar_t Trs = L"$Trs";

		constexpr const wchar_t TDOnce = L"$TDOnce";
		constexpr const wchar_t TDEx = L"$TDEx";
		constexpr const wchar_t Inverse = L"$Inverse";
	}
	std::variant<FormulasFile, ParserFile, TrsFile, std::monostate> FileParser::parse(const std::wstring& string, const std::wstring& filePath)
	{
		m_str = std::wstring_view(string);
		TokenMatcher tokenMatcher({
			Tokens::Parser,
			Tokens::Formulas,
			Tokens::Trs,
			});

		m_chPos = 0;

		if (auto match = tokenMatcher.findNext(string, m_chPos))
		{
			auto token = tokenMatcher.tokens()[match.value().tokenIndex];
			if (token == Tokens::Parser)
			{
				return ParserFile();
			}
			else if (token == Tokens::Formulas)
			{
				return FormulasFile();
			}
			else if (token == Tokens::Trs)
			{
				return TrsFile();
			}
		}
		return std::monostate;
	}

	ParserFile FileParser::handleParserFile()
	{
		//pos points right after $Parser
		ParserFile res;
		TokenMatcher tokenMatcher({
			Tokens::TDOnce,
			Tokens::TDEx,
			Tokens::Inverse
			});
		bool inverse = false;
		auto startMatch = tokenMatcher.findNext(m_str, m_chPos);
		if (!startMatch || tokenMatcher.tokens()[startMatch.value().tokenIndex] == Tokens::Inverse)
		{
			return {};
		}
		std::optional<TokenMatcher::Match> nextMatch;
		while (true)
		{

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
