#pragma once
#include "PatternMatchingHelpers.h"
#include "BracketsParser.h"

namespace TryAlgebraCore::Trs
{

	class Transformer
	{
	public:
		void removeContainers(std::vector<std::unique_ptr<TermIntermediate>>& subj);
		void addContainers(std::vector<std::unique_ptr<TermIntermediate>>& subj);
	private:
		bool isTrm(const std::wstring& label) const;
		BracketsParser bracketsParser;
	};
}