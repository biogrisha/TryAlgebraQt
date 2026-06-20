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

	void recognizeVariables(std::vector<TermRawSh>& pattern)
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

	void convertMeToTerms(const std::span<std::unique_ptr<MeBase>>& meList, std::vector<TermRawSh>& result, TermRaw* parent)
	{
		for (auto& me : meList)
		{
			TermRawSh newTerm = std::make_shared<TermRaw>();
			newTerm->label = me->getName();
			newTerm->parent = parent;
			result.push_back(newTerm);
			convertMeToTerms(me->getChildren(), newTerm->children, newTerm.get());
		}
	}

	void unifyVariables(std::vector<TermRawSh>& terms, std::vector<TermRaw*>& variables)
	{
		for (auto& tr : terms)
		{
			if (tr->variable)
			{
				for (auto vr : variables)
				{
					if (compare(tr.get(), vr))
					{
						tr.reset(vr);
					}
				}
			}
			else
			{
				unifyVariables(tr->children, variables);
			}
		}
	}

	bool PatternMatcher::match(const std::vector<Term*>& pattern, const std::span<Term*>& terms)
	{
		while (true)
		{
			
		}
		return true;
	}

	bool compareWithVariable(TermRaw* var, const std::span<TermRawSh>& terms)
	{
		if (var->captured.size() != terms.size())
		{
			return false;
		}
		for (int i = 0; i < terms.size(); ++i)
		{
			if (!compare(var->captured[i].get(), terms[i].get()))
			{
				return false;
			}
		}
		return true;
	}

	bool compare(TermRaw* lhs, TermRaw* rhs)
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
			if (!compare(lhs->children[i].get(), rhs->children[i].get()))
			{
				return false;
			}
		}
		return true;
	}

	PatternMatcher::State::State(std::vector<TermRawSh>& pat, std::vector<TermRawSh>& terms)
		: pat(pat)
		, terms(terms)
		, variator(1,1)
	{
	}

	void PatternMatcher::State::variatorStep()
	{
		variator.step();

	}

	bool PatternMatcher::State::compBoundaries()
	{
		{
			int termsId = 0;
			for (int patId = 0; patId < pat.size(); ++patId)
			{
				if (pat[patId]->variable)
				{
					if (pat[patId]->captured.empty())
					{
						//reached empty variable
						variablesStartPat = patId;
						variablesStartTerms = termsId;
						break;
					}
					else
					{
						//variable captured something
						if (!compareWithVariable(pat[patId].get(), std::span(terms).subspan(termsId, pat[patId]->captured.size())))
						{
							//captured sequence not equal to the one in terms
							return false;
						}
						//compared sequences are equal -> make step to the number of captured elements
						termsId += pat[patId]->captured.size();
					}
					continue;
				}
				else if (pat[patId]->pattern && pat[patId]->label != terms[patId]->label)
				{
					//pattern and labels are not equal -> fail
					return false;
				}
				else if (!pat[patId]->pattern && !compare(pat[patId].get(), terms[patId].get()))
				{
					//ground terms and not equal -> fail
					return false;
				}
				++termsId;
			}
		}

		{
			int termsId = terms.size() - 1;
			for (int patId = pat.size() - 1; patId >= 0; --patId)
			{
				if (pat[patId]->variable)
				{
					int captureSize = pat[patId]->captured.size();
					if (captureSize == 0)
					{
						//reached empty variable
						variablesEndPat = patId + 1;
						variablesEndTerms = termsId + 1;
						break;
					}
					else
					{
						//variable captured something
						if (!compareWithVariable(pat[patId].get()
							, std::span(terms).subspan(termsId - captureSize + 1, captureSize)))
						{
							//captured sequence not equal to the one in terms
							return false;
						}
						//compared sequences are equal -> make step to the number of captured elements
						termsId -= captureSize;
					}
					continue;
				}
				else if (pat[patId]->pattern && pat[patId]->label != terms[patId]->label)
				{
					//pattern and labels are not equal -> fail
					return false;
				}
				else if (!pat[patId]->pattern && compare(pat[patId].get(), terms[patId].get()))
				{
					//ground terms and not equal -> fail
					return false;
				}
				--termsId;
			}
		}
		return true;
	}

	bool PatternMatcher::State::compIntermediate()
	{

		return false;
	}	

	Variator::Variator(int size, int sum)
	{
		m_sum = sum;
		m_offsets = std::vector<int>(size - 1, 0);
		//first set offsets to 1,3,5...
		for (int i = 0; i < m_offsets.size(); ++i)
		{
			m_offsets[i] = i + 1;
		}
	}

	int Variator::step()
	{
		if (m_isFirstStep)
		{
			m_isFirstStep = true;
			return 0;
		}

		for (int i = m_offsets.size() - 1; i >= 0; --i)
		{
			if (!isLastPos(m_offsets, m_sum, i))
			{
				int changedFrom = i;
				++m_offsets[i];
				int baseOffset = m_offsets[i];
				++i;
				int j = 1;
				for (; i < m_offsets.size(); ++i)
				{
					m_offsets[i] = j + baseOffset;
					++j;
				}
				return changedFrom;
			}
		}
		return 0;
	}

	bool Variator::isLastPos(const std::vector<int>& offsets, const int sum, const int i)
	{
		//size 4, sum 10, i 3, exp 9 -> 10 - 1 - (4 - 1 - 3)
		return offsets[i] == sum - 1 - (offsets.size() -  1 - i);
	}

	std::vector<int> Variator::generateSizes(const std::vector<int>& offsets, const int sum)
	{
		std::vector<int> sizes;
		if (offsets.empty())
		{
			sizes.push_back(sum);
			return sizes;
		}
		int lastVal = 0;
		for (auto offset : offsets)
		{
			sizes.push_back(offset - lastVal);
			lastVal = offset;
		}
		sizes.push_back(sum - offsets.back());
		return sizes;
	}

}
