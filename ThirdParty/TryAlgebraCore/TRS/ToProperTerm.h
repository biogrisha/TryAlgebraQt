#pragma once
#include <string>
#include <memory>
#include <vector>
#include "PatternMatchingHelpers.h"
#include <MathEditor/include/TextBuffer.h>
#include "BinaryOperatorParser.h"
#include "Trs.h"
#include "TrsParser.h"

namespace TryAlgebraCore::Trs
{
	class ToProperTerm
	{
	public:
		void run(const std::wstring& str);
		const std::vector<std::unique_ptr<TermIntermediate>>& get() const;
		void setup(const TextBuffer& tb);

	private:
		void toTerm(const std::unique_ptr<TermIntermediate>& from, NewTrs::Term*& to, NewTrs::Term* parent = nullptr);
		void toIntermediate(NewTrs::Term* term, std::unique_ptr<TermIntermediate>& intermediate);
		std::optional<std::wstring> getSection(const TextBuffer& tb, const std::wstring& sectionName);
		bool waitToken(TextBufferIterator& it, const std::wstring& token);
		void print(const std::vector<std::unique_ptr<TermIntermediate>>& terms);
		std::vector<std::unique_ptr<TermIntermediate>> m_terms;
		Transformer m_transformer;
		TrsIdentitiesParser m_trsIdentitiesParser;
		char m_ch = 100;
		std::unordered_map<std::wstring, char> m_symbols;
		std::unordered_map<char, std::wstring> m_symbolsInv;

	};
}