#pragma once
#include "PatternMatchingHelpers.h"
#include <MathEditor/include/TextBuffer.h>
#include <string_view>
#include <unordered_set>
#include "TermTransformer.h"
#include <variant>

namespace TryAlgebraCore::Trs
{
	struct Formula
	{
		std::wstring name;
		std::vector<std::vector<std::unique_ptr<TermIntermediate>>> equality;
	};
	struct FormulasFile
	{
		std::wstring filePath;
		std::wstring parserFilePath;
		//formulas - formula - terms sequence
		std::vector<Formula> formulas;
	};

	struct ParserFile
	{
		std::wstring filePath;
		std::vector<RewritingRule> rules;
		std::vector<RewritingRule> invRules;
	};

	struct TrsFile
	{
		std::wstring filePath;
		std::vector<IdentityIntermediate> rules;
	};

	class FileParser
	{
		struct ParserRule
		{
			std::vector<std::unique_ptr<TermIntermediate>> from;
			std::vector<std::unique_ptr<TermIntermediate>> to;
		};

	public:
		std::variant<FormulasFile, ParserFile, TrsFile, std::monostate> parse(const std::wstring& string, const std::wstring& filePath);
	private:
		ParserFile handleParserFile();
		FormulasFile handleFormulasFile();
		bool waitToken(const std::wstring& token);
		std::vector<ParserRule> parseParsingRules(const std::wstring_view& str);
		std::vector<std::vector<std::unique_ptr<TermIntermediate>>> parseFormula(const std::wstring_view& str);
		std::wstring_view m_str;
		int m_pos = 0;
	};

}