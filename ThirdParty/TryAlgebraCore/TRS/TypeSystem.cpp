#include "TypeSystem.h"

namespace TryAlgebraCore::Trs
{

	InfixLeftAssocParser::InfixLeftAssocParser(std::vector<std::unique_ptr<Trs::TermIntermediate>>&& pat,
		std::vector<std::unique_ptr<Trs::TermIntermediate>>&& ret)
		: m_pat(std::move(pat))
		, m_ret(std::move(ret))
	{
		markVariables(m_pat);
		markVariables(m_ret);
		unifyVariables(m_pat, m_patContext);
		unifyVariables(m_ret, m_patContext);
		markPatternNodes(m_pat);
		generateLevels(m_pat, m_patContext);
		for (auto& b : m_patContext.levels.front().bundles)
		{
			m_patContext.bundles.push_back(&b);
		}
	}

	void TypeSystem::addOperatorParser(std::unique_ptr<OperatorParser>&& operatorParser)
	{
		m_operatorParsers.push_back(std::move(operatorParser));
	}

}
