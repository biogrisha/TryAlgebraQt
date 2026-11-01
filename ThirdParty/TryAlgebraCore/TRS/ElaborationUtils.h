#pragma once
#include "PatternMatchingHelpers.h"

namespace TryAlgebraCore::Trs::ElaborationUtils
{
	void commonTermElaboration(std::vector<std::unique_ptr<TermIntermediate>>& terms);
	std::wstring termsToString(const std::span<std::unique_ptr<TermIntermediate>>& terms);
}