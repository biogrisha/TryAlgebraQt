#include "TermTransformer.h"
#include <Me/include/MeGlobals.h>
#include <iostream>
namespace TryAlgebraCore::Trs
{

	void Transformer::removeContainers(std::vector<std::unique_ptr<TermIntermediate>>& subj)
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
	void Transformer::addContainers(std::vector<std::unique_ptr<TermIntermediate>>& subj)
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

	bool Transformer::isTrm(const std::wstring& l) const
	{
		return l.size() > 2 && l[0] == L't' && l[1] == L'r' && l[2] == L'm';
	}
}
