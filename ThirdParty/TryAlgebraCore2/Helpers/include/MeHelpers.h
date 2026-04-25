#pragma once
#include <string>
#include <vector>
#include <Me/include/MeBase.h>

namespace TryAlgebraCore2
{
	class MeHelpers
	{
	public:
		std::vector<std::unique_ptr<MeBase>> parseFromString(const std::wstring& str, class MeGenerator* generator);
	};
}