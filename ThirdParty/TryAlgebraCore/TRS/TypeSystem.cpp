#include "TypeSystem.h"
#include <Me/include/MeGlobals.h>

namespace TryAlgebraCore::Trs
{
	namespace {
		constexpr const wchar_t* opLabel = L"op";
	}
	InfixLeftAssocParser::InfixLeftAssocParser(std::vector<std::unique_ptr<Trs::TermIntermediate>>&& pat,
		std::vector<std::unique_ptr<Trs::TermIntermediate>>&& ret)
		: m_ret(std::move(ret))
	{
		assert(pat.size() > 3);
		assert(pat.front()->label == MeNames::typeAnnotation);
		assert(pat.back()->label == MeNames::typeAnnotation);

		m_pat.push_back(std::move(pat.front()));
		std::unique_ptr<TermIntermediate> op = std::make_unique<TermIntermediate>();
		op->label = opLabel;
		for (int i = 1; i < pat.size() - 1; ++i)
		{
			pat[i]->parent = op.get();
			op->children.push_back(std::move(pat[i]));
		}
		m_op = op.get();
		m_pat.push_back(std::move(op));
		m_pat.push_back(std::move(pat.back()));

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

	std::vector<std::unique_ptr<Trs::TermIntermediate>> InfixLeftAssocParser::getOperator() const
	{
		return {};
	}

	void TypeSystem::addOperatorParser(std::unique_ptr<OperatorParser>&& operatorParser)
	{
		m_operatorParsers.push_back(std::move(operatorParser));
	}

}
