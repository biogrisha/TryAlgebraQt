#include "TermElaborator.h"
#include <Me/include/MeGlobals.h>
#include <iostream>
namespace TryAlgebraCore::Trs
{
	namespace {
		constexpr const wchar_t* argLabel = L"arg";
	}

	TermElaborator::TermElaborator(std::unique_ptr<OperatorElaborator>&& operatorElaborator)
		:m_operatorElaborator(std::move(operatorElaborator))
	{

	}

	void TermElaborator::removeContainers(std::vector<std::unique_ptr<TermIntermediate>>& subj)
	{
		for (int i = 0; i < subj.size(); ++i)
		{
			if (subj[i]->isVariable)
			{
				continue;
			}
			removeContainers(subj[i]->children);
			if (subj[i]->label == MeNames::cont)
			{
				if (!subj[i]->children.empty())
				{
					subj[i] = std::move(subj[i]->children.back());
				}
			}
		}
	}
	void TermElaborator::addContainers(std::vector<std::unique_ptr<TermIntermediate>>& subj)
	{
		for (int i = 0; i < subj.size(); ++i)
		{
			addContainers(subj[i]->children);
			if (isTrm(subj[i]->label))
			{
				auto children = std::move(subj[i]->children);
				subj[i]->children.clear();
				for (auto& ch : children)
				{
					auto& cont = subj[i]->children.emplace_back(std::make_unique<TermIntermediate>());
					cont->label = MeNames::cont;
					cont->children.push_back(std::move(ch));
				}
			}
		}
	}

	void TermElaborator::applyDirect(std::vector<std::unique_ptr<TermIntermediate>>& subj, bool cosmetic)
	{
		m_bracketsElaborator.elaborateBrackets(subj);
		elaborateArgs(subj);
		m_operatorElaborator->apply(subj);
	}

	bool TermElaborator::isTrm(const std::wstring& l) const
	{
		return l.size() > 2 && l[0] == L't' && l[1] == L'r' && l[2] == L'm';
	}
	void TermElaborator::elaborateArgs(std::vector<std::unique_ptr<TermIntermediate>>& subj, TermIntermediate* parent)
	{
		for (auto& t : subj)
		{
			elaborateArgs(t->children, t.get());
		}
		if (!parent || parent->label != L"br")
		{
			return;
		}
		std::vector<std::unique_ptr<TermIntermediate>> res;
		int from = 0;
		auto insertTerms = [&res, &subj](int from, int to)
			{
				if (from >= to)
				{
					return;
				}
				auto arg = std::make_unique<TermIntermediate>();
				arg->label = argLabel;
				for (int i = from; i < to; ++i)
				{
					arg->children.push_back(std::move(subj[i]));
				}
				res.push_back(std::move(arg));
			};
		for (int i = 0; i < subj.size(); ++i)
		{
			if (subj[i]->label == L",")
			{
				insertTerms(from, i);
				from = i + 1;
			}
		}
		insertTerms(from, subj.size());
		subj = std::move(res);
	}
}
