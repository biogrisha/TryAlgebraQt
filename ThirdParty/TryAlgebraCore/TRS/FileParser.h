#pragma once
#include "PatternMatchingHelpers.h"
#include <MathEditor/include/TextBuffer.h>
#include <string_view>
#include <unordered_set>

namespace TryAlgebraCore::Trs
{
	struct ParsingRules
	{
		std::wstring importKeyword;
		std::wstring formulaKeyword;
		std::unordered_set<std::wstring> identitySections;
		std::vector<std::wstring> tokens;
	};

	struct FileSection
	{
		enum class Type
		{
			Keyword,
			ParsingRule,
			TrsRule,
			SyntacticFormula,
			Formula,
		};
		std::wstring meta;
		std::vector<std::vector<std::unique_ptr<TermIntermediate>>>	identities;
	};

	class FileParser
	{
	public:
		void parse(const std::wstring& string, const ParsingRules& rules);
	private:
		bool waitToken(const std::wstring& token);
		std::vector<std::vector<std::unique_ptr<TermIntermediate>>> parseIdentities(const std::wstring_view& str);
		std::vector<FileSection> m_sections;
		std::wstring m_str;
		int m_chPos = 0;
	};

}