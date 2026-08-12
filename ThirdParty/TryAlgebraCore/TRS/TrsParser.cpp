#include "TrsParser.h"

namespace TryAlgebraCore::Trs
{
	void TrsIdentitiesParser::setup(const std::wstring& str)
	{
		m_identities = parseIdentities(str);
		for (auto& id : m_identities)
		{
			markVariables(id.lhs);
			markVariables(id.rhs);
		}
	}
	void TrsIdentitiesParser::refine(Transformer& parser)
	{
		for (auto& id : m_identities)
		{
			parser.applyAll(id.lhs);
			parser.applyAll(id.rhs);
		}
	}
	const std::vector<IdentityIntermediate>& TrsIdentitiesParser::identities() const
	{
		return m_identities;
	}
}
