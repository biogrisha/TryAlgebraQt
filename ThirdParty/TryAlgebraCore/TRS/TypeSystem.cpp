#include "TypeSystem.h"
#include <Me/include/MeGlobals.h>
#include <span>
namespace TryAlgebraCore::Trs
{
	namespace {
		constexpr const wchar_t* opLabel = L"op";
	}
	InfixLeftAssocParser::InfixLeftAssocParser(std::vector<std::unique_ptr<Trs::TermIntermediate>>&& pat,
		std::vector<std::unique_ptr<Trs::TermIntermediate>>&& ret)
	{
		assert(pat.size() > 3);
		assert(pat.front()->label == MeNames::typeAnnotation);
		assert(pat.back()->label == MeNames::typeAnnotation);
		markVariables(pat);
		markVariables(ret);

		std::vector<std::unique_ptr<TermIntermediate>> patWithOp;
		std::unique_ptr<TermIntermediate> op = std::make_unique<TermIntermediate>();
		op->label = opLabel;
		for (int i = 1; i < pat.size() - 1; ++i)
		{
			pat[i]->parent = op.get();
			op->children.push_back(std::move(pat[i]));
		}
		std::vector<std::unique_ptr<TermIntermediate>> opCopy;
		opCopy.push_back(copyTermIntermediate(op));
		auto opChildren = copyTermIntermediate(op->children);
		patWithOp.push_back(std::move(pat.front()));
		patWithOp.push_back(std::move(op));
		patWithOp.push_back(std::move(pat.back()));

		m_operatorPatId = m_rewriteOperator.addPat(std::move(opChildren));
		m_operatorInstId = m_rewriteOperator.addInst(std::move(opCopy));
		m_operationPatId = m_rewriteOperation.addPat(std::move(patWithOp));
		m_operationInstId = m_rewriteOperation.addInst(std::move(ret));
	}

	void InfixLeftAssocParser::apply(std::vector<std::unique_ptr<Trs::TermIntermediate>>& subj)
	{
		for (auto& t : subj)
		{
			markOperator(t->children);
		}
		rewriteLeftRightOneTime(subj, m_rewriteOperation, m_operationPatId, m_operationInstId);
	}

	void InfixLeftAssocParser::markOperator(std::vector<std::unique_ptr<Trs::TermIntermediate>>& subj)
	{
		for (auto& t : subj)
		{
			markOperator(t->children);
		}
		rewriteLeftRightOneTime(subj, m_rewriteOperator, m_operatorPatId, m_operatorInstId);
	}

	void TypeSystem::addOperatorParser(std::unique_ptr<OperatorParser>&& operatorParser, int precedence)
	{
		m_operatorParsers[precedence].push_back(std::move(operatorParser));
	}

	void TypeSystem::apply(std::vector<std::unique_ptr<Trs::TermIntermediate>>& subj)
	{
		for (auto& [precedence, parsers] : m_operatorParsers)
		{
			for (auto& parser : parsers)
			{
				parser->markOperator(subj);
			}
		}
		for (auto& [precedence, parsers] : m_operatorParsers)
		{
			for (auto& parser : parsers)
			{
				parser->apply(subj);
			}
		}
	}
}
