#include "FileParser.h"
#include "TokenMatcher.h"
#include <Me/include/MeCharacter.h>

namespace TryAlgebraCore::Trs
{
	namespace Tokens
	{
		constexpr const wchar_t* FormulaStart = L"$$";
		constexpr const wchar_t* Import = L"$Import";
		constexpr const wchar_t* Parser = L"$Parser";
		constexpr const wchar_t* Formulas = L"$Formulas";
		constexpr const wchar_t* Trs = L"$Trs";

		constexpr const wchar_t* TDOnce = L"$TDOnce";
		constexpr const wchar_t* TDEx = L"$TDEx";
		constexpr const wchar_t* Inverse = L"$Inverse";
	}
	namespace {
		void trim(std::wstring& s)
		{
			const auto first = s.find_first_not_of(L" \t\n\r");
			if (first == std::wstring::npos) {
				s.clear();
				return;
			}

			const auto last = s.find_last_not_of(L" \t\n\r");
			s = s.substr(first, last - first + 1);
		}
	}
	std::variant<FormulasFile, ParserFile, TrsFile, std::monostate> FileParser::parse(const std::wstring& string, const std::wstring& filePath)
	{
		m_str = std::wstring_view(string);
		TokenMatcher tokenMatcher({
			Tokens::Parser,
			Tokens::Formulas,
			Tokens::Trs,
			});

		m_pos = 0;

		if (auto match = tokenMatcher.findNext(string, m_pos))
		{
			auto token = tokenMatcher.tokens()[match.value().tokenIndex];
			if (token == Tokens::Parser)
			{
				auto parserFile = handleParserFile();
				parserFile.filePath = filePath;
				return parserFile;
			}
			else if (token == Tokens::Formulas)
			{
				return handleFormulasFile();
			}
			else if (token == Tokens::Trs)
			{
				return TrsFile();
			}
		}
		return std::monostate{};
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

		{
			//direct rules
			auto lastMatch = tokenMatcher.findNext(m_str, m_pos);
			while (m_pos < m_str.size())
			{
				auto newMatch = tokenMatcher.findNext(m_str, m_pos);
				if (tokenMatcher.tokens()[newMatch.value().tokenIndex] == Tokens::Inverse)
				{
					break;
				}
				auto& token = tokenMatcher.tokens()[lastMatch.value().tokenIndex];
				RuleType type =
					token == Tokens::TDOnce ? RuleType::TDSimpleRecursive :
					token == Tokens::TDEx ? RuleType::TDRecursiveExhausting : RuleType::None;
				if (type == RuleType::None)
				{
					return {};
				}
				int from = lastMatch.value().endCharIndex;
				int size = 0;
				if (newMatch)
				{
					size = newMatch.value().endCharIndex - tokenMatcher.tokens()[newMatch.value().tokenIndex].size() - from;
				}
				else
				{
					size = m_pos - from;
				}
				auto rules = parseParsingRules(std::wstring_view(m_str).substr(from, size));
				for (auto& rule : rules)
				{
					auto& newRule = res.rules.emplace_back();
					newRule.from = std::move(rule.from);
					newRule.to = std::move(rule.to);
					newRule.type = type;
				}
				lastMatch = newMatch;
			}
		}
		{
			//inverse rules
			auto lastMatch = tokenMatcher.findNext(m_str, m_pos);
			while (m_pos < m_str.size())
			{
				auto newMatch = tokenMatcher.findNext(m_str, m_pos);
				auto& token = tokenMatcher.tokens()[lastMatch.value().tokenIndex];
				RuleType type =
					token == Tokens::TDOnce ? RuleType::TDSimpleRecursive :
					token == Tokens::TDEx ? RuleType::TDRecursiveExhausting : RuleType::None;
				if (type == RuleType::None)
				{
					return {};
				}
				int from = lastMatch.value().endCharIndex;
				int size = 0;
				if (newMatch)
				{
					size = newMatch.value().endCharIndex - tokenMatcher.tokens()[newMatch.value().tokenIndex].size() - from;
				}
				else
				{
					size = m_pos - from;
				}
				auto rules = parseParsingRules(std::wstring_view(m_str).substr(from, size));
				for (auto& rule : rules)
				{
					auto& newRule = res.invRules.emplace_back();
					newRule.from = std::move(rule.from);
					newRule.to = std::move(rule.to);
					newRule.type = type;
				}
				lastMatch = newMatch;
			}
		}
		return res;
	}

	FormulasFile FileParser::handleFormulasFile()
	{
		FormulasFile res;

		//find import path
		if (waitToken(Tokens::Import))
		{
			int from = m_pos;
			if (waitToken(L"\n"))
			{
				--m_pos;
			}
			res.parserFilePath = m_str.substr(from, m_pos - from);
			trim(res.parserFilePath);
		}
		else
		{
			m_pos = 0;
		}

		if (!waitToken(Tokens::FormulaStart))
		{
			return {};
		}
		while (true)
		{
			int from = m_pos;
			std::wstring name;
			for (; m_pos < m_str.size(); ++m_pos)
			{
				if (m_str[m_pos] == L'\n')
				{
					name.insert(name.begin(), m_str.begin() + from, m_str.begin() + m_pos);
					++m_pos;
					break;
				}
			}
			from = m_pos;
			bool hasNext = waitToken(Tokens::FormulaStart);
			int size = hasNext ? m_pos - from - std::wstring(Tokens::FormulaStart).size() : m_pos - from;
			auto equality = parseFormula(m_str.substr(from, size));
			if (!equality.empty())
			{
				auto& formula = res.formulas.emplace_back();
				formula.equality = std::move(equality);
				formula.name = std::move(name);
			}
			if (!hasNext)
			{
				break;
			}
		}
		return res;
	}

	bool FileParser::waitToken(const std::wstring& token)
	{
		int progress = 0;

		while (m_pos < m_str.size())
		{
			wchar_t ch = m_str[m_pos];
			m_pos++;
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
		return progress == token.size();
	}

	std::vector<FileParser::ParserRule> FileParser::parseParsingRules(const std::wstring_view& str)
	{
		std::vector<ParserRule> res;
		auto terms = parseToTermIntermediate(str);

		ParserRule currentRule;
		bool isFrom = true;
		for (auto& term : terms)
		{
			if (term->label == L"\n")
			{
				if (!currentRule.from.empty() && !currentRule.to.empty())
				{
					res.push_back(std::move(currentRule));
				}
				isFrom = true;
				currentRule = {};
				continue;
			}

			if (term->label == L" ")
			{
				continue;
			}

			if (term->label == L"=")
			{
				isFrom = false;
				continue;
			}
			if (isFrom)
			{
				currentRule.from.push_back(std::move(term));
			}
			else
			{
				currentRule.to.push_back(std::move(term));
			}
		}
		return res;
	}

	std::vector<std::vector<std::unique_ptr<TermIntermediate>>> FileParser::parseFormula(const std::wstring_view& str)
	{
		std::unordered_set<std::wstring> clear = {
			L"\n",
			L" "
		};
		auto terms = parseToTermIntermediate(str);
		std::vector<std::vector<std::unique_ptr<TermIntermediate>>> res;
		std::vector<std::unique_ptr<TermIntermediate>> sequence;
		for (auto& term : terms)
		{
			if (clear.contains(term->label))
			{
				continue;
			}
			if (term->label == L"=")
			{
				if (sequence.empty())
				{
					return {};
				}
				res.push_back(std::move(sequence));
				sequence.clear();
				continue;
			}
			sequence.push_back(std::move(term));
		}
		if (sequence.empty())
		{
			return {};
		}
		res.push_back(std::move(sequence));
		return res;
	}

}
