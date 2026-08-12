#pragma once
#include "PatternMatchingHelpers.h"

namespace TryAlgebraCore::Trs
{
	class BracketsParser
	{
	public:
		void apply(std::vector<std::unique_ptr<TermIntermediate>>& subj);
		void removeBrackets(std::vector<std::unique_ptr<TermIntermediate>>& subj);
	};
}