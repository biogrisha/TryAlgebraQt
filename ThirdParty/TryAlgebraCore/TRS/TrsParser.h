#pragma once
#include <string>
#include "BinaryOperatorParser.h"

namespace TryAlgebraCore::Trs
{
	class TrsIdentitiesParser
	{
	public:
		void setup(const std::wstring& str);
		void refine(Transformer& parser);
		const std::vector<IdentityIntermediate>& identities() const;
	private:
		std::vector<IdentityIntermediate> m_identities;
	};
}