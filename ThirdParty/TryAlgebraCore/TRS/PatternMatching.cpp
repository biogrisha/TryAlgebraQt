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
				bool found = false;
				for (auto vr : variables)
				{
					if (compare(tr.get(), vr))
					{
						tr->captured = vr->captured;
						found = true;
						break;
					}
				}
				if (!found)
				{
					variables.push_back(tr.get());
				}
			}
			else
			{
				unifyVariables(tr->children, variables);
			}
		}
	}

	bool compareUntilCheckpoint(PatternMatcher::State* state
		, TermRaw* topParent
		, const std::span<TermRawSh>& topLevelPat
		, const std::span<TermRawSh>& topLevelTerms)
	{
		for (int i = state->cachedPatternsId; i < state->cachedPatterns.size(); ++i)
		{

		}
		return false;
	}

	bool PatternMatcher::match(std::vector<TermRawSh>& pat, const std::span<TermRawSh>& terms)
	{
		if (hasVariable(pat))
		{
			State st(pat, terms);
			if (!st.compBoundaries())
			{
				return false;
			}
			st.setVariablesTime();
			if (!st.nextVariation())
			{
				return false;
			}

		}
		return true;
	}

	bool PatternMatcher::back()
	{
		return false;
	}

	bool hasVariable(const std::vector<TermRawSh>& pat)
	{
		for (auto& term : pat)
		{
			if (term->variable && term->captured->empty())
			{
				return true;
			}
		}
		return false;
	}

	bool compareWithVariable(TermRaw* var, const std::span<TermRawSh>& terms)
	{
		if (var->captured->size() != terms.size())
		{
			return false;
		}
		for (int i = 0; i < terms.size(); ++i)
		{
			if (!compare(var->captured->operator[](i).get(), terms[i].get()))
			{
				return false;
			}
		}
		return true;
	}

	CompUntilCheckpoint compUntilCheckpoint(TermRaw* pat, TermRaw* term)
	{
		auto& patChildren = pat->children;
		auto& termChildren = term->children;
		int termsId = 0;
		for (int patId = 0; patId < patChildren.size(); ++patId)
		{
			if (patChildren[patId]->variable)
			{
				if (patChildren[patId]->captured->empty())
				{
					assert(false);
				}
				else
				{
					//variable captured something
					if (!compareWithVariable(patChildren[patId].get()
						, std::span(termChildren).subspan(termsId, patChildren[patId]->captured->size())))
					{
						//captured sequence not equal to the one in terms
						CompUntilCheckpoint res;
						res.failed = true;
						return res;
					}
					//compared sequences are equal -> make step to the number of captured elements
					termsId += patChildren[patId]->captured->size();
				}
				continue;
			}
			else if (patChildren[patId]->pattern)
			{
				//if (!compUntilCheckpoint(patChildren[patId].get(), termChildren[termsId].get()))
				//{
				//	CompUntilCheckpoint res;
				//	res.failed = true;
				//	return res;

				//}

				//pattern and labels are not equal -> fail
			}
			else if (!patChildren[patId]->pattern && !compare(patChildren[patId].get(), termChildren[termsId].get()))
			{
				CompUntilCheckpoint res;
				res.failed = true;
				return res;
			}
			++termsId;
		}
		CompUntilCheckpoint res;
		return res;
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

	PatternMatcher::State::State(const std::span<TermRawSh>& pat, const std::span<TermRawSh>& terms)
		: pat(pat)
		, terms(terms)
	{
		time = staticTime;
		++staticTime;
	}

	bool PatternMatcher::State::handleVariatorStep()
	{
		auto sizes = variator->getSizes();
		int termsId = variablesStartTerms;
		int sizeId = 0;
		for (int patId = variablesStartPat; patId < variablesEndPat; ++patId)
		{
			if (pat[patId]->variable)
			{
				if (pat[patId]->time == time)
				{
					auto span = std::span(terms).subspan(termsId, sizes[sizeId]);
					if (pat[patId]->captured->empty())
					{
						*(pat[patId]->captured) = span;
					}
					else if (!compareWithVariable(pat[patId].get(), span))
					{
						return false;
					}
					++sizeId;
				}
				else if (!compareWithVariable(pat[patId].get(), std::span(terms).subspan(termsId, pat[patId]->captured->size())))
				{
					return false;
				}
				termsId += pat[patId]->captured->size();
				continue;
			}
			else if (pat[patId]->pattern)
			{
				if (pat[patId]->label != terms[termsId]->label)
				{
					//pattern and labels are not equal -> fail
					return false;
				}
				pat[patId]->patMatch = terms[termsId].get();
			}
			else if (!pat[patId]->pattern && !compare(pat[patId].get(), terms[termsId].get()))
			{
				return false;
			}
			++termsId;
		}
		return true;
	}

	bool PatternMatcher::State::nextVariation()
	{
		while (variator->step())
		{
			if (handleVariatorStep());
			{
				return true;
			}
		}
		return false;
	}

	void PatternMatcher::State::setVariablesTime()
	{
		for (int i = variablesStartPat; i < variablesEndPat; ++i)
		{
			if (pat[i]->variable && pat[i]->captured->empty())
			{
				pat[i]->time = time;
			}
		}
	}

	bool PatternMatcher::State::compBoundaries()
	{
		{
			int termsId = 0;
			for (int patId = 0; patId < pat.size(); ++patId)
			{
				if (pat[patId]->variable)
				{
					if (pat[patId]->captured->empty())
					{
						//reached empty variable
						variablesStartPat = patId;
						variablesStartTerms = termsId;
						break;
					}
					else
					{
						//variable captured something
						if (!compareWithVariable(pat[patId].get(), std::span(terms).subspan(termsId, pat[patId]->captured->size())))
						{
							//captured sequence not equal to the one in terms
							return false;
						}
						//compared sequences are equal -> make step to the number of captured elements
						termsId += pat[patId]->captured->size();
					}
					continue;
				}
				else if (pat[patId]->pattern)
				{
					if (pat[patId]->label != terms[termsId]->label)
					{
						//pattern and labels are not equal -> fail
						return false;
					}
					pat[patId]->patMatch = terms[termsId].get();
				}
				else if (!pat[patId]->pattern && !compare(pat[patId].get(), terms[termsId].get()))
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
					int captureSize = pat[patId]->captured->size();
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
				else if (pat[patId]->pattern)
				{
					if (pat[patId]->label != terms[termsId]->label)
					{
						//pattern and labels are not equal -> fail
						return false;
					}
					pat[patId]->patMatch = terms[termsId].get();
				}
				else if (!pat[patId]->pattern && !compare(pat[patId].get(), terms[termsId].get()))
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
		int termsId = variablesStartTerms;
		for (int patId = 0; patId < pat.size(); ++patId)
		{
			if (pat[patId]->pattern)
			{

			}
		}
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

	bool Variator::step()
	{
		if (m_lastStep)
		{
			return false;
		}
		if (m_isFirstStep)
		{
			m_isFirstStep = true;
			m_lastStep = finished(m_offsets, m_sum);
			return true;
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
				break;
			}
		}
		m_lastStep = finished(m_offsets, m_sum);
		return true;
	}

	bool Variator::finished(const std::vector<int>& offsets, const int sum)
	{
		if (offsets.empty())
		{
			return true;
		}
		return isLastPos(offsets, sum, 0);
	}

	std::vector<int> Variator::getSizes()
	{
		return generateSizes(m_offsets, m_sum);
	}

	bool Variator::isLastPos(const std::vector<int>& offsets, const int sum, const int i)
	{
		//size 4, sum 10, i 3, exp 9 -> 10 - 1 - (4 - 1 - 3)
		return offsets[i] == sum - 1 - (offsets.size() - 1 - i);
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
