#pragma once

#include "TestHelpers.h"
#include "TestGlobals.h"
#include <TRS/PatternMatching.h>
#include <iostream>
#include <span>
namespace PatternMatchingTest
{
	struct TermTest;
	struct VariableMeta
	{
		std::span<std::unique_ptr<TermTest>> captured;
		bool isDetermined = false;
		bool isCaptured = false;
	};

	struct TermTest
	{
		std::wstring label;
		std::vector<std::unique_ptr<TermTest>> children;
		TermTest* parent = nullptr;
		TermTest* subj = nullptr;
		bool isVariable = false;
		bool isPattern = false;
		std::shared_ptr<VariableMeta> variableMeta;
		int num = 0;
	};

	struct Block
	{
		std::span<std::unique_ptr<TermTest>> terms;
	};

	struct PathEl
	{
		int pos = 0;
		bool fromLeft = true;
	};

	struct Level
	{
		std::vector<Block> blocks;
		std::vector<std::vector<PathEl>> determinedVars;
	};


	inline std::vector<std::unique_ptr<TermTest>> topPat;
	class Parser
	{
	public:
		Parser(const std::wstring& str)
			: m_str(str)
		{

		}

		void parse()
		{
			while (m_pos != m_str.size())
			{
				int term_start = m_pos;
				consumeTermName();
				int label_end = m_pos;
				auto t = new TermTest();
				m_current_term = t;
				if (m_parent_term)
				{
					m_current_term->parent = m_parent_term;
					m_parent_term->children.push_back(std::unique_ptr<TermTest>(m_current_term));
				}
				if (m_pos > m_str.size())
				{
					return;
				}
				if (m_str[m_pos] == '(')
				{
					++m_pos;
					m_parent_term = m_current_term;
					parse();
					m_current_term = m_parent_term;
					m_parent_term = m_current_term->parent;
				}
				m_current_term->label = m_str.substr(term_start, label_end - term_start);
				if (m_pos >= m_str.size())
				{
					return;
				}
				if (m_str[m_pos] == ')')
				{
					++m_pos;
					return;
				}
				++m_pos;
			}
		}

		void consumeTermName()
		{
			int i = m_pos;
			for (; i < m_str.size(); ++i)
			{
				if (m_str[i] == '(' || m_str[i] == ')' || m_str[i] == ',')
				{
					break;
				}
			}
			m_pos = i;
		}

		TermTest* m_current_term = nullptr;
		TermTest* m_parent_term = nullptr;
		std::wstring m_str;
		int m_pos = 0;
	};

	inline bool compare(TermTest* lhs, TermTest* rhs)
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

	inline void unifyVariables(std::vector<std::unique_ptr<TermTest>>& terms, std::vector<TermTest*>& variables)
	{
		for (auto& tr : terms)
		{
			if (tr->isVariable)
			{
				bool found = false;
				for (auto vr : variables)
				{
					if (compare(tr.get(), vr))
					{
						tr->variableMeta = vr->variableMeta;
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

	inline void enumerate(std::vector<std::unique_ptr<TermTest>>& pat)
	{
		for (int i = 0; i < pat.size(); ++i)
		{
			pat[i]->num = i;
			enumerate(pat[i]->children);
		}
	}

	void markPatternNodes(TermTest* t)
	{
		bool pat_temp = false;
		for (auto ch : t->children)
		{
			markPatternNodes(ch);
			pat_temp |= ch->pat;
		}
		if (pat_temp)
		{
			t->pat = true;
			return;
		}
		if (t->variable)
		{
			t->pat = true;
		}
	}

	inline std::vector<PathEl> inversePath(TermTest* term)
	{
		std::vector<PathEl> path;

		while (true)
		{
			PathEl el;
			el.pos = term->num;
			auto& pat = (term->parent == nullptr ? topPat : term->parent->children);
			for (int i = 0; i < el.pos; ++i)
			{
				if (pat[i]->isVariable && !pat[i]->variableMeta->isCaptured && !pat[i]->variableMeta->isDetermined)
				{
					el.fromLeft = false;
					break;
				}
			}
			path.push_back(el);
			term = term->parent;
			if (!term)
			{
				break;
			}
		}
		return path;
	}

	inline void collectBlocks(std::vector<std::unique_ptr<TermTest>>& pat, std::vector<Block>& blocks
		, std::vector<std::vector<PathEl>>& determinedVars)
	{
		int varStart = -1;
		int varEnd = -1;
		//find varStart
		for (int i = 0; i < pat.size(); ++i)
		{
			if (pat[i]->isVariable && !pat[i]->variableMeta->isDetermined && !pat[i]->variableMeta->isCaptured)
			{
				varStart = i;
				break;
			}
		}

		if (varStart == -1)
		{
			//no variable found -> recursive call for all patterns
			for (auto& el : pat)
			{
				if (el->isPattern)
				{
					collectBlocks(el->children, blocks, determinedVars);
				}
			}
			return;
		}
		// find var end (last var pos + 1)
		for (int i = pat.size() - 1; i >= varStart; --i)
		{
			if (pat[i]->isVariable && !pat[i]->variableMeta->isDetermined && !pat[i]->variableMeta->isCaptured)
			{
				varEnd = i + 1;
				break;
			}
		}

		if (varEnd - varStart == 1)
		{
			//single variable on the level - it is determined(no variation needed)
			//mark as determined and call recursion on all patterns
			pat[varStart]->variableMeta->isDetermined = true;
			determinedVars.push_back(inversePath(pat[varStart].get()));
			for (auto& el : pat)
			{
				if (el->isPattern)
				{
					collectBlocks(el->children, blocks, determinedVars);
				}
			}
		}

		//this level has multiple yet not determined variables
		//create block for them
		Block block;
		block.terms = std::span(pat).subspan(varStart, varEnd - varStart);
		blocks.push_back(block);

		//recursive call for all patterns on the left and on the right
		for (int i = 0; i < varStart; ++i)
		{
			if (pat[i]->isPattern)
			{
				collectBlocks(pat[i]->children, blocks, determinedVars);
			}
		}
		for (int i = varEnd; i < pat.size(); ++i)
		{
			if (pat[i]->isPattern)
			{
				collectBlocks(pat[i]->children, blocks, determinedVars);
			}
		}
	}

	inline bool removeDetermined(std::vector<Block>& blocks, std::vector<std::vector<PathEl>>& determinedVars)
	{
		bool res = false;
		for (int blI = blocks.size() - 1; blI >= 0; --blI)
		{
			auto& block = blocks[blI];
			int varStart = -1;
			int varEnd = -1;
			for (int i = 0; i < block.terms.size(); ++i)
			{
				auto& el = block.terms[i];
				if (el->isVariable && !el->variableMeta->isDetermined && !el->variableMeta->isCaptured)
				{
					varStart = i;
				}
			}
			if (varStart == -1)
			{
				res = true;
				//block is determined, call collect blocks on pats in it
				for (auto& el : block.terms)
				{
					if (el->isPattern)
					{
						collectBlocks(el->children, blocks, determinedVars);
					}
				}
				std::swap(blocks[blI], blocks.back());
				blocks.pop_back();
				continue;
			}
			for (int i = block.terms.size() - 1; i >= varStart; --i)
			{
				auto& el = block.terms[i];
				if (el->isVariable && !el->variableMeta->isDetermined && !el->variableMeta->isCaptured)
				{
					varEnd = i + 1;
				}
			}
			if (varEnd - varStart == 1)
			{
				res = true;
				for (auto& el : block.terms)
				{
					if (el->isPattern)
					{
						collectBlocks(el->children, blocks, determinedVars);
					}
				}
				block.terms[varStart]->variableMeta->isDetermined;
				std::swap(blocks[blI], blocks.back());
				blocks.pop_back();
				continue;
			}
			if (varEnd - varStart != block.terms.size())
			{
				res = true;
				for (int i = 0; i < varStart; ++i)
				{
					if (block.terms[i]->isPattern)
					{
						collectBlocks(block.terms[i]->children, blocks, determinedVars);
					}
				}
				for (int i = varEnd; i < block.terms.size(); ++i)
				{
					if (block.terms[i]->isPattern)
					{
						collectBlocks(block.terms[i]->children, blocks, determinedVars);
					}
				}
				block.terms = block.terms.subspan(varStart, varEnd - varStart);
			}

		}
		return res;
	}

	inline void func(std::vector<std::unique_ptr<TermTest>>& pat)
	{
		std::vector<Level> levels;
		{
			std::vector<Block> blocks;
			std::vector<std::vector<PathEl>> determinedVars;
			collectBlocks(pat, blocks, determinedVars);
			while (removeDetermined(blocks, determinedVars));
			for (auto& bl : blocks)
			{
				for (auto& el : bl.terms)
				{
					if (el->isVariable)
					{
						el->variableMeta->isCaptured = true;
					}
				}
			}
			levels.emplace_back(std::move(blocks), std::move(determinedVars));
		}

		while (true)
		{
			std::vector<Block> blocks;
			std::vector<std::vector<PathEl>> determinedVars;
			for (auto& bl : levels.back().blocks)
			{
				for (auto& el : bl.terms)
				{
					if (el->isPattern)
					{
						collectBlocks(el->children, blocks, determinedVars);
					}
				}
			}
			while (removeDetermined(blocks, determinedVars));
			for (auto& bl : blocks)
			{
				for (auto& el : bl.terms)
				{
					if (el->isVariable)
					{
						el->variableMeta->isCaptured = true;
					}
				}
			}
			if (blocks.empty())
			{
				break;
			}
			levels.emplace_back(std::move(blocks), std::move(determinedVars));
		}

	}

	void determineVar(const std::vector<int>& path, std::vector<std::unique_ptr<TermTest>>& pat, std::vector<std::unique_ptr<TermTest>>& subj, int i = 0)
	{
		if (i = path.size() - 1)
		{
			//find capture from
			int from = 0;
			for (int patI = 0; patI < pat.size(); ++patI)
			{
				if (pat[patI]->isVariable)
				{
					from += pat[patI]->variableMeta->captured.size();
				}
			}
		}
		if (pat[i]->subj)
		{

		}

	}
	void func2(const std::vector<Level>& levels, std::vector<std::unique_ptr<TermTest>>& pat, std::vector<std::unique_ptr<TermTest>>& subj)
	{
		for (const auto& level : levels)
		{
			for (const auto& path : level.determinedVars)
			{

			}
		}

	}


	void markVariables(const std::unique_ptr<TermTest>& term)
	{
		if (term->label[0] == L'`')
		{
			term->isVariable = true;
			term->variableMeta = std::make_shared<VariableMeta>();
			return;
		}
		for (const auto& child : term->children)
		{
			markVariables(child);
		}
	}

	MYTEST(VariatorTest)
	{
		Parser parser(L"f(a(k,`d,g),`b)");
		parser.parse();
		std::unique_ptr<TermTest> mainTerm = std::unique_ptr<TermTest>(parser.m_current_term);
		std::vector<std::unique_ptr<TermTest>> str;
		markVariables(mainTerm);
		str.push_back(std::move(mainTerm));
		enumerate(str);
		std::vector<TermTest*> vars;
		unifyVariables(str, vars);

	}
}