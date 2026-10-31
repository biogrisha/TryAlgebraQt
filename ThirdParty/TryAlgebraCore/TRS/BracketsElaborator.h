#pragma once
#include "PatternMatchingHelpers.h"

namespace TryAlgebraCore::Trs
{
	class CommonElaborator
	{
	public:
		static void elaborateBrackets(std::vector<std::unique_ptr<TermIntermediate>>& subj);
		static void elaborateArguments()
			static void removeBrackets(std::vector<std::unique_ptr<TermIntermediate>>& subj);
	};
}