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
		bool isPureVar()
		{
			return isVariable
				&& variableMeta.get() != nullptr
				&& !variableMeta->isCaptured
				&& !variableMeta->isDetermined;
		}
	};

	struct Block
	{
		std::span<std::unique_ptr<TermTest>> terms;
		std::vector<TermTest*> vars;
	};

	struct PathEl
	{
		int pos = 0;
		bool fromLeft = true;
	};

	struct Level
	{
		Block* findBlock(const TermTest* parent)
		{
			for (auto& block : blocks)
			{
				if (block.terms.back()->parent == parent)
				{
					return &block;
				}
			}
			return nullptr;
		}
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

	bool markPatternNodes(std::vector<std::unique_ptr<TermTest>>& t)
	{
		bool res = false;
		for (const auto& el : t)
		{
			if (el->isVariable)
			{
				res = true;
			}
			else if (markPatternNodes(el->children))
			{
				res = true;
				el->isPattern = true;
			}
		}
		return res;
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
			if (pat[i]->isPureVar())
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
			if (pat[i]->isPureVar())
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
			return;
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
				if (el->isPureVar())
				{
					varStart = i;
					break;
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
				if (el->isPureVar())
				{
					varEnd = i + 1;
					break;
				}
			}
			if (varEnd - varStart == 1)
			{
				res = true;
				block.terms[varStart]->variableMeta->isDetermined = true;
				determinedVars.push_back(inversePath(block.terms[varStart].get()));
				for (auto& el : block.terms)
				{
					if (el->isPattern)
					{
						collectBlocks(el->children, blocks, determinedVars);
					}
				}
				//iterating in decreasing order -> can safely swap-pop
				std::swap(blocks[blI], blocks.back());
				blocks.pop_back();
				continue;
			}
			{
				//check that variables are different
				TermTest* firstVar = nullptr;
				bool sameVariable = true;
				for (int i = varStart; i < varEnd; ++i)
				{
					if (block.terms[i]->isPureVar())
					{
						if (!firstVar)
						{
							firstVar = block.terms[i].get();
						}
						else if (!compare(firstVar, block.terms[i].get()))
						{
							sameVariable = false;
							break;
						}
					}
				}

				if (sameVariable)
				{
					res = true;
					block.terms[varStart]->variableMeta->isDetermined = true;
					determinedVars.push_back(inversePath(block.terms[varStart].get()));
					for (auto& el : block.terms)
					{
						if (el->isPattern)
						{
							collectBlocks(el->children, blocks, determinedVars);
						}
					}
					//iterating in decreasing order -> can safely swap-pop
					std::swap(blocks[blI], blocks.back());
					blocks.pop_back();
					continue;
				}
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

	void collectVariables(const std::span<std::unique_ptr<TermTest>>& block, std::vector<TermTest*>& vars)
	{
		for (const auto& t : block)
		{
			if (t->isPattern)
			{
				collectVariables(t->children, vars);
			}
			else if (t->isPureVar())
			{
				vars.push_back(t.get());
			}
		}
	}

	inline std::vector<Level> func(std::vector<std::unique_ptr<TermTest>>& pat)
	{
		std::vector<Level> levels;
		{
			std::vector<Block> blocks;
			std::vector<std::vector<PathEl>> determinedVars;
			collectBlocks(pat, blocks, determinedVars);
			while (removeDetermined(blocks, determinedVars));
			for (auto& bl : blocks)
			{
				collectVariables(bl.terms, bl.vars);
			}
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
				collectVariables(bl.terms, bl.vars);
			}
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
				levels.emplace_back(std::move(blocks), std::move(determinedVars));
				break;
			}
			levels.emplace_back(std::move(blocks), std::move(determinedVars));
		}
		return levels;
	}

	bool determineVar(const std::vector<PathEl>& path, std::vector<std::unique_ptr<TermTest>>& pat, std::vector<std::unique_ptr<TermTest>>& subj, int i = 0)
	{
		if (i < path.size() - 1)
		{
			//i is not pointing at variable - intermediate index
			if (pat[path[i].pos]->subj)
			{
				//pat already has assigned subj -> dive right in
				return determineVar(path, pat[path[i].pos]->children, pat[path[i].pos]->subj->children, i + 1);
			}
			//find capture from
			int subjPos = path[i].fromLeft ? 0 : subj.size();
			if (path[i].fromLeft)
			{
				//path from left -> accumulate left to right
				for (int patI = 0; patI < path[i].pos; ++patI)
				{
					if (pat[patI]->isVariable)
					{
						subjPos += pat[patI]->variableMeta->captured.size();
					}
					else
					{
						++subjPos;
					}
				}
			}
			else
			{
				//path from right -> accumulate right to left
				for (int patI = pat.size() - 1; patI >= path[i].pos; --patI)
				{
					if (pat[patI]->isVariable)
					{
						subjPos -= pat[patI]->variableMeta->captured.size();
					}
					else
					{
						--subjPos;
					}
				}
			}
			if (subjPos >= subj.size() || subjPos < 0)
			{
				//subj pos is outside subj range
				return false;
			}
			//successfuly found subjPos
			//assign subj to pat
			pat[path[i].pos]->subj = subj[subjPos].get();
			//recursive step
			return determineVar(path, pat[path[i].pos]->children, subj[subjPos]->children, i + 1);
		}
		else
		{
			//final pos pointing at variable
			int subjFrom = 0;
			int patFrom = 0;
			int subjTo = subj.size();
			int patTo = 0;

			//there could be one or multiple same variables
			//find first occurance of variable in pat and corresponding start in subj
			for (int i = 0; i < pat.size(); ++i)
			{
				if (pat[i]->isVariable)
				{
					if (pat[i]->variableMeta->captured.empty())
					{
						//found not initialized variable
						//cache position
						patFrom = i;
						break;
					}
					else
					{
						//found initialized variable -> skip n terms in subj
						subjFrom += pat[i]->variableMeta->captured.size();
					}
				}
				else
				{
					//skip 1 term in subj
					++subjFrom;
				}
			}
			if (subjFrom >= subjTo)
			{
				//subj start goes over subj size
				return false;
			}
			//find last variable and subj arguments end
			for (int i = pat.size() - 1; i >= patFrom; ++i)
			{
				if (pat[i]->isVariable)
				{
					if (pat[i]->variableMeta->captured.empty())
					{
						patTo = i + 1;
						break;
					}
					else
					{
						subjTo -= pat[i]->variableMeta->captured.size();
					}
				}
				else
				{
					--subjFrom;
				}
			}
			if (subjFrom <= subjTo)
			{
				//invalid subj end
				return false;
			}
			//calculate number of variables
			int varNum = 0;
			for (int i = patFrom; i < patTo; ++i)
			{
				if (pat[i]->isVariable && pat[i]->variableMeta->captured.empty())
				{
					++varNum;
				}
			}
			//calc free args number
			int freeArgsNum = (subjTo - subjFrom) - (patTo - patFrom - varNum);

			if (freeArgsNum % varNum != 0)
			{
				//number of free arguments is not divided by the number of variables
				return false;
			}
			//number of arguments captured by variable
			int argsPerVarNum = freeArgsNum / varNum;
			//assign variables to the first variable
			pat[patFrom]->variableMeta->captured = std::span(subj).subspan(subjFrom, argsPerVarNum);
		}
		return true;
	}

	bool compare1(Level& level, std::vector<std::unique_ptr<TermTest>>& pat, std::vector<std::unique_ptr<TermTest>>& subj)
	{
		int start = -1;
		int end = -1;
		int subjI = 0;
		if (pat.size() > subj.size())
		{
			return false;
		}

		//compare left border
		for (int patI = 0; patI < pat.size(); ++patI)
		{
			if (pat[patI]->isPattern)
			{
				if (pat[patI]->label != subj[subjI]->label)
				{
					return false;
				}
				if (!compare1(level, pat[patI]->children, subj[subjI]->children))
				{
					return false;
				}
			}
			else if (pat[patI]->isVariable)
			{
				if (!pat[patI]->variableMeta->captured.empty())
				{
					for (auto& cap : pat[patI]->variableMeta->captured)
					{
						if (!compare(cap.get(), subj[subjI].get()))
						{
							return false;
						}
						++subjI;
					}
					//continue to avoid subjI incrementation
					continue;
				}
				else
				{
					//found block start
					start = subjI;
					break;
				}
			}
			else if (!compare(pat[patI].get(), subj[subjI].get()))
			{
				return false;
			}
			++subjI;
		}
		if (start == -1)
		{
			return true;
		}
		//compare right border
		for (int patI = pat.size() - 1; patI >= start; --patI)
		{
			if (pat[patI]->isPattern)
			{
				if (pat[patI]->label != subj[subjI]->label)
				{
					return false;
				}
				if (!compare1(level, pat[patI]->children, subj[subjI]->children))
				{
					return false;
				}
			}
			else if (pat[patI]->isVariable)
			{
				if (!pat[patI]->variableMeta->captured.empty())
				{
					for (auto& cap : pat[patI]->variableMeta->captured)
					{
						if (!compare(cap.get(), subj[subjI].get()))
						{
							return false;
						}
						--subjI;
					}
				}
				else
				{
					//found block start
					end = subjI + 1;
					break;
				}
			}
			else if (!compare(pat[patI].get(), subj[subjI].get()))
			{
				return false;
			}
			--subjI;
		}

	}

	bool func2(std::vector<Level>& levels, std::vector<std::unique_ptr<TermTest>>& pat, std::vector<std::unique_ptr<TermTest>>& subj, int levelI = 0)
	{
		auto& level = levels[levelI];
		//first determine variables
		for (const auto& path : level.determinedVars)
		{
			if (!determineVar(path, pat, subj))
			{
				return false;
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


	void print(const std::span<std::unique_ptr<TermTest>>& terms)
	{
		for (int i = 0; i < terms.size(); ++i)
		{
			std::wcout << terms[i]->label;
			if (!terms[i]->children.empty())
			{
				std::wcout << L"(";
				print(terms[i]->children);
				std::wcout << L")";
			}
			if (i < terms.size() - 1)
			{
				std::wcout << L",";
			}
		}
	}


	MYTEST(VariatorTest)
	{
		auto s = L"t(f(a,b,`x,k(`d,`k),`x,t),f1(t))";
		Parser parser(s);
		parser.parse();
		std::unique_ptr<TermTest> mainTerm = std::unique_ptr<TermTest>(parser.m_current_term);
		std::vector<std::unique_ptr<TermTest>> str;
		markVariables(mainTerm);
		str.push_back(std::move(mainTerm));
		enumerate(str);
		std::vector<TermTest*> vars;
		unifyVariables(str, vars);
		markPatternNodes(str);
		auto levels = func(str);


		std::wcout << s << "\n";
		for (auto& lev : levels)
		{
			std::cout << "\n=========level========\n";
			for (auto& bl : lev.blocks)
			{
				std::cout << "======block\n";
				std::cout << "vars\n";
				for (auto var : bl.vars)
				{
					std::wcout << var->label << ",";
				}
				std::cout << "\n";
				std::cout << "block \n";
				print(bl.terms);
				std::cout << "\n";
			}
			std::cout << "\n=========determined========\n";
			for (auto& path : lev.determinedVars)
			{
				std::cout << "[";
				for (auto& pos : path)
				{
					std::cout << pos.pos << "(" << (pos.fromLeft ? "L" : "R") << ")";
				}
				std::cout << "]\n";
			}
		}


	}
}