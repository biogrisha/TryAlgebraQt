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

	bool PatternMatcher::State::compBoundaries()
	{
		std::optional<int> variableStart;
		{
			int termsId = 0;
			for (int patId = 0; patId < pat.size(); ++patId)
			{
				if (pat[patId]->variable)
				{
					if (!pat[patId]->captured.empty())
					{
						//reached empty variable
						variableStart = patId;
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
				else if (!pat[patId]->pattern && compare(pat[patId].get(), terms[patId].get()))
				{
					//ground terms and not equal -> fail
					return false;
				}
				++termsId;
			}
		}

		if(!variableStart.has_value())
		{
			return true;
		}

		std::optional<int> variableEnd;
		{
			int termsId = terms.size() - 1;
			for (int patId = pat.size() - 1; patId >= 0; -- patId)
			{
				if (pat[patId]->variable)
				{
					if (!pat[patId]->captured.empty())
					{
						//reached empty variable
						variableStart = patId;
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
				else if (!pat[patId]->pattern && compare(pat[patId].get(), terms[patId].get()))
				{
					//ground terms and not equal -> fail
					return false;
				}
				++termsId;
			}
		}
	}

	bool PatternMatcher::State::compIntermediate()
	{

		return false;
	}	

	Variator::Variator(int size, int sum)
	{
		m_sum = sum + size - 2;
		m_offsets = std::vector<int>(size - 1);
		for (int i = 0; i < m_offsets.size(); ++i)
		{
			m_offsets[i] = i;
		}
		m_sizes = std::vector<int>(size, 0);
		m_sizes[0] = sum;
	}

	int Variator::step()
	{
		bool needReset = false;
		for (int i = 0; i < m_offsets.size(); ++i)
		{
			int pos = m_offsets.size() - 1 - i;
			if(m_offsets[pos] == m_sum - i)
			{
				needReset = true;
				continue;
			}
			
			if (m_offsets[pos] != m_sum - i)
			{
				m_offsets[pos] += 1;
				int size = m_offsets[pos];
				for (int j = 1; j + pos < m_offsets.size(); ++j)
				{
					m_offsets[j + pos] = size + j;
				}
				break;
			}
		}
		m_sizes.clear();
		
		int lastEnd = m_sum;
		for (int i = m_offsets.size() - 1; i >= 0; --i)
		{
			m_sizes.push_back(lastEnd - m_offsets[i]);
			lastEnd = m_offsets[i] - (i == 0 ? 0 : 1);
		}
		m_sizes.push_back(lastEnd);
		if (m_sizes.back() == m_sum - m_sizes.size() + 2)
		{
			return 1;
		}
		return 0;
	}

}
