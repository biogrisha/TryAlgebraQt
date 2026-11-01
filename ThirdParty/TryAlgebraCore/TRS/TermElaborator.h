#pragma once
#include "PatternMatchingHelpers.h"
#include "ElaborationUtils.h"
#include "OperatorElaborator.h"
namespace TryAlgebraCore::Trs
{

	class TermElaborator
	{
	public:
		explicit TermElaborator(std::unique_ptr<OperatorElaborator>&& operatorElaborator);
		void applyDirect(std::vector<std::unique_ptr<TermIntermediate>>& subj, bool cosmetic);
	private:
		void removeContainers(std::vector<std::unique_ptr<TermIntermediate>>& subj);
		void addContainers(std::vector<std::unique_ptr<TermIntermediate>>& subj);
		bool isTrm(const std::wstring& label) const;
		std::unique_ptr<OperatorElaborator> m_operatorElaborator;
	};
}