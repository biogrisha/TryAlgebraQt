#pragma once
#include <TRS/PatternMatchingHelpers.h>
#include <span>

namespace TryAlgebraCore::Trs
{
	class OperatorParser
	{
	public:
		virtual ~OperatorParser() = default;
	};

	class InfixLeftAssocParser : public OperatorParser
	{
	public:
		InfixLeftAssocParser(std::vector<std::unique_ptr<Trs::TermIntermediate>>&& pat,
			std::vector<std::unique_ptr<Trs::TermIntermediate>>&& ret);
		void apply(std::vector<std::unique_ptr<Trs::TermIntermediate>>& subj) {};
		std::vector<std::unique_ptr<Trs::TermIntermediate>> getOperator() const;
	private:
		Trs::PatContext m_patContext;
		std::vector<std::unique_ptr<Trs::TermIntermediate>> m_pat;
		std::vector<std::unique_ptr<Trs::TermIntermediate>> m_ret;
		Trs::TermIntermediate* m_op;
	};

	class TypeSystem
	{
	public:
		void addOperatorParser(std::unique_ptr<OperatorParser>&& operatorParser);
	private:
		std::vector<std::unique_ptr<OperatorParser>> m_operatorParsers;
	};
}