#pragma once
#include <TRS/PatternMatchingHelpers.h>

namespace TryAlgebraCore::Trs
{
	class OperatorParser
	{

	};

	class InfixLeftAssocParser : public OperatorParser
	{
	public:
		InfixLeftAssocParser(std::vector<std::unique_ptr<Trs::TermIntermediate>>&& pat,
			std::vector<std::unique_ptr<Trs::TermIntermediate>>&& ret);
		void apply(std::vector<std::unique_ptr<Trs::TermIntermediate>>& subj) {};
	private:

		Trs::PatContext m_patContext;
		std::vector<std::unique_ptr<Trs::TermIntermediate>> m_pat;
		std::vector<std::unique_ptr<Trs::TermIntermediate>> m_ret;
	};

	class TypeSystem
	{

	};
}