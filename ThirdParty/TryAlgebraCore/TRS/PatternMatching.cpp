#include "PatternMatching.h"
#include <Me/include/MeContainer.h>
#include <Me/include/MeGlobals.h>

namespace TryAlgebraCore::Trs
{
	bool match(const std::span<Term*>& terms, const std::vector<Term*>& pattern, int from)
	{
		if (terms.size() < pattern.size())
		{
			return false;
		}
		for (int i = 0; i < pattern.size(); ++i)
		{
			if (pattern[i]->variable)
			{

			}
		}
		return true;
	}

	void recognizeVariables(std::vector<Term*>& pattern)
	{
		for (auto tr : pattern)
		{
			if (tr->label == MeNames::variable)
			{
				tr->variable = true;
			}
			else
			{
				recognizeVariables(tr->children);
			}
		}

	}

	void convertMeToTerms(const std::span<std::unique_ptr<MeBase>>& meList, std::vector<Term*>& result, Term* parent)
	{
		for (auto& me : meList)
		{
			Term* newTerm = new Term;
			newTerm->label = me->getName();
			if(parent)
			{
				newTerm->parents.insert(parent);
			}
			result.push_back(newTerm);
			convertMeToTerms(me->getChildren(), newTerm->children, newTerm);
		}
	}

	void unifyVariables(std::vector<Term*>& terms, std::vector<Term*>& variables)
	{
		for (auto& tr : terms)
		{
			if (tr->variable)
			{
				for (auto vr : variables)
				{
					if (compare(tr, vr))
					{
						deleteRecursive(tr);
						tr = vr;
					}
				}
			}
			else
			{
				unifyVariables(tr->children, variables);
			}
		}
	}

	void clearReps(std::vector<Term*>& terms)
	{
		for (auto tr : terms)
		{
			clearReps(tr->children);
			tr->e_reps.clear();
		}
	}

	bool PatternMatcher::match(const std::vector<Term*>& pattern, const std::span<Term*>& terms)
	{
		while (true)
		{
			
		}
	}

	bool compare(Term* lhs, Term* rhs)
	{
		if (lhs->label != rhs->label)
		{
			return false;
		}
		if (lhs->children.size() != rhs->children.size())
		{
			return false;
		}
		for (int i = 0; i < lhs->children.size(); ++i)
		{
			if (!compare(lhs->children[i], rhs->children[i]))
			{
				return false;
			}
		}
		return true;
	}

	bool PatternMatcher::State::compBoundaries()
	{
		for (int i = 0; i < groundStartNum; ++i)
		{
			if (pat[i]->variable)
			{

			}
			if (!compare(terms[i], pat[i]))
			{
				return false;
			}
		}

		for (int i = 0; i < groundEndNum; ++i)
		{
			if (!compare(terms[terms.size() - 1 - i], pat[pat.size() - 1 - i]))
			{
				return false;
			}
		}
		return true;
	}

	bool PatternMatcher::State::compIntermediate()
	{

		return false;
	}

}
