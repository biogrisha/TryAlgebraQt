#pragma once
#include <string>
#include <span>
#include <memory>
#include <vector>
#include "PatternMatchingHelpers.h"
#include <MathEditor/include/TextBuffer.h>
#include "BinaryOperatorParser.h"

namespace TryAlgebraCore::Trs
{
	class ToProperTerm
	{
	public:
		void run(const std::wstring& str);
		const std::vector<std::unique_ptr<TermIntermediate>>& get() const;
		void setup(const TextBuffer& tb);
	private:
		std::optional<std::wstring> getSection(const TextBuffer& tb, const std::wstring& sectionName);
		bool waitToken(TextBufferIterator& it, const std::wstring& token);
		std::vector<std::unique_ptr<TermIntermediate>> m_terms;
		BinaryOperatorParser binaryOperatorParser;
	};
}