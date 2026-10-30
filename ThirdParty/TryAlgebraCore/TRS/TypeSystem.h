#pragma once
#include <TRS/PatternMatchingHelpers.h>
#include <map>

namespace TryAlgebraCore::Trs
{
	class OperatorParser
	{
	public:
		virtual ~OperatorParser() = default;
		virtual void apply(std::vector<std::unique_ptr<Trs::TermIntermediate>>& subj) {};
		virtual void markOperator(std::vector<std::unique_ptr<Trs::TermIntermediate>>& subj) {};
	};

	class InfixLeftAssocParser : public OperatorParser
	{
	public:
		InfixLeftAssocParser(std::vector<std::unique_ptr<Trs::TermIntermediate>>&& pat,
			std::vector<std::unique_ptr<Trs::TermIntermediate>>&& ret);
		void apply(std::vector<std::unique_ptr<Trs::TermIntermediate>>& subj) override;
		void markOperator(std::vector<std::unique_ptr<Trs::TermIntermediate>>& subj) override;
	private:
		Trs::PatternMatcher m_rewriteOperation;
		Trs::PatternMatcher m_rewriteOperator;
		Trs::PatternMatcher::Id m_operatorPatId = 0;
		Trs::PatternMatcher::Id m_operatorInstId = 0;
		Trs::PatternMatcher::Id m_operationPatId = 0;
		Trs::PatternMatcher::Id m_operationInstId = 0;

	};

	class TypeSystem
	{
	public:
		void addOperatorParser(std::unique_ptr<OperatorParser>&& operatorParser, int precedence);
		void apply(std::vector<std::unique_ptr<Trs::TermIntermediate>>& subj);
	private:
		std::map<int, std::vector<std::unique_ptr<OperatorParser>>> m_operatorParsers;
	};
}