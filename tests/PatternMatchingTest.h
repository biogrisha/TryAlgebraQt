#pragma once

#include "TestHelpers.h"
#include "TestGlobals.h"
#include <TRS/PatternMatching.h>
#include <iostream>
#include <span>
#include <unordered_map>

namespace PatternMatchingTest
{
	struct TermTest;
	struct VariableMeta
	{
		std::span<std::unique_ptr<TermTest>> captured;
		bool isDetermined = false;
		bool isCaptured = false;
		int captureSizeNondet = 0;
	};

	struct EqVar
	{
		VariableMeta* var = nullptr;
		int coef = 0;
	};

	struct Equation
	{
		std::vector<EqVar> prevVars;
		std::vector<EqVar> initVars;
		int rhs = 0;

		std::unique_ptr<Equation> next;
	};

	enum class Status
	{
		finished,
		exceeded,
		succeeded
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
		std::span<std::unique_ptr<TermTest>> pat;
		std::unordered_set<TermTest*> varsRec;
		std::vector<TermTest*> vars;
		std::span<std::unique_ptr<TermTest>> subj;
	};

	struct PathEl
	{
		int pos = 0;
		bool fromLeft = true;
	};

	struct Bundle
	{
		std::vector<Block*> blocks;
		Equation eq;
		std::vector<Bundle*> children;
	};

	struct Level
	{
		Block* findBlock(const TermTest* parent)
		{
			for (auto& block : blocks)
			{
				if (block.pat.back()->parent == parent)
				{
					return &block;
				}
			}
			return nullptr;
		}
		std::vector<Block> blocks;
		std::vector<std::vector<PathEl>> determinedVars;
		std::vector<Bundle> bundles;
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
		block.pat = std::span(pat).subspan(varStart, varEnd - varStart);
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
			for (int i = 0; i < block.pat.size(); ++i)
			{
				auto& el = block.pat[i];
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
				for (auto& el : block.pat)
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
			for (int i = block.pat.size() - 1; i >= varStart; --i)
			{
				auto& el = block.pat[i];
				if (el->isPureVar())
				{
					varEnd = i + 1;
					break;
				}
			}
			if (varEnd - varStart == 1)
			{
				res = true;
				block.pat[varStart]->variableMeta->isDetermined = true;
				determinedVars.push_back(inversePath(block.pat[varStart].get()));
				for (auto& el : block.pat)
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
					if (block.pat[i]->isPureVar())
					{
						if (!firstVar)
						{
							firstVar = block.pat[i].get();
						}
						else if (!compare(firstVar, block.pat[i].get()))
						{
							sameVariable = false;
							break;
						}
					}
				}

				if (sameVariable)
				{
					res = true;
					block.pat[varStart]->variableMeta->isDetermined = true;
					determinedVars.push_back(inversePath(block.pat[varStart].get()));
					for (auto& el : block.pat)
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

			if (varEnd - varStart != block.pat.size())
			{
				res = true;
				for (int i = 0; i < varStart; ++i)
				{
					if (block.pat[i]->isPattern)
					{
						collectBlocks(block.pat[i]->children, blocks, determinedVars);
					}
				}
				for (int i = varEnd; i < block.pat.size(); ++i)
				{
					if (block.pat[i]->isPattern)
					{
						collectBlocks(block.pat[i]->children, blocks, determinedVars);
					}
				}
				block.pat = block.pat.subspan(varStart, varEnd - varStart);
			}

		}
		return res;
	}
	void collectVariables(Block& bl)
	{
		for (auto& t : bl.pat)
		{
			if (t->isPureVar())
			{
				bl.vars.push_back(t.get());
			}
		}
	}

	void collectVariablesRec(const std::span<std::unique_ptr<TermTest>>& block, std::unordered_set<TermTest*>& vars)
	{
		for (const auto& t : block)
		{
			if (t->isPattern)
			{
				collectVariablesRec(t->children, vars);
			}
			else if (t->isPureVar())
			{
				vars.insert(t.get());
			}
		}
	}

	void collectBundles(std::vector<Level>& levels)
	{
		for (auto& level : levels)
		{
			Bundle bundle;
			std::vector<Block*> blocksCopy;
			for (auto& block : level.blocks)
			{
				blocksCopy.push_back(&block);
			}
			while (!blocksCopy.empty())
			{
				//start bundle
				auto& bundle = level.bundles.emplace_back();
				bundle.blocks.push_back(std::move(blocksCopy.back()));
				blocksCopy.pop_back();

				//try to find a block among the remaining ones
				for (int candidateI = blocksCopy.size() - 1; candidateI >= 0; --candidateI)
				{
					bool hasSameVar = false;
					//iterate over blocks in last bundle
					for (const auto* bundleBl : bundle.blocks)
					{
						//iterate over variables in all blocks of bundle
						for (const auto& bundleVar : bundleBl->varsRec)
						{
							//iterate over variables in candidate block
							for (const auto& candidateVar : blocksCopy[candidateI]->varsRec)
							{
								if (bundleVar->variableMeta == candidateVar->variableMeta)
								{
									//found same variable
									hasSameVar = true;
									break;
								}
							}
							if (hasSameVar)
							{
								break;
							}
						}
						if (hasSameVar)
						{
							break;
						}
					}
					if (hasSameVar)
					{
						//add block into bundle
						bundle.blocks.push_back(std::move(blocksCopy[candidateI]));
						std::swap(blocksCopy.back(), blocksCopy[candidateI]);
						blocksCopy.pop_back();
						//start process from the first candidate 
						//since the newly added candidate can connect to the one before it
						candidateI = blocksCopy.size();
					}
				}
			}


		}
	}

	inline void initDiophantineEq(Bundle& bundle)
	{
		std::unordered_set<VariableMeta*> vars;
		Equation* eq = &bundle.eq;
		for (int i = 0; i < bundle.blocks.size(); ++i)
		{
			auto* block = bundle.blocks[i];
			std::unordered_map<VariableMeta*, int> initVars;
			std::unordered_map<VariableMeta*, int> prevVars;
			for (auto& var : block->vars)
			{
				if (vars.contains(var->variableMeta.get()))
				{
					prevVars[var->variableMeta.get()]++;
				}
				else
				{
					initVars[var->variableMeta.get()]++;
				}
			}
			for (auto& var : block->vars)
			{
				vars.insert(var->variableMeta.get());
			}

			for (auto& initVar : initVars)
			{
				eq->initVars.emplace_back(initVar.first, initVar.second);
			}
			for (auto& prevVar : prevVars)
			{
				eq->prevVars.emplace_back(prevVar.first, prevVar.second);
			}
			if (i < bundle.blocks.size() - 1)
			{
				eq->next = std::make_unique<Equation>();
				eq = eq->next.get();
			}
		}
	}

	inline std::vector<Level> generateLevels(std::vector<std::unique_ptr<TermTest>>& pat)
	{
		std::vector<Level> levels;
		{
			std::vector<Block> blocks;
			std::vector<std::vector<PathEl>> determinedVars;
			collectBlocks(pat, blocks, determinedVars);
			while (removeDetermined(blocks, determinedVars));
			for (auto& bl : blocks)
			{
				collectVariables(bl);
				collectVariablesRec(bl.pat, bl.varsRec);
			}
			for (auto& bl : blocks)
			{
				for (auto& el : bl.pat)
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
				for (auto& el : bl.pat)
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
				collectVariables(bl);
				collectVariablesRec(bl.pat, bl.varsRec);
			}
			for (auto& bl : blocks)
			{
				for (auto& el : bl.pat)
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

		collectBundles(levels);
		for (auto& level : levels)
		{
			for (auto& bundle : level.bundles)
			{
				initDiophantineEq(bundle);
			}
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
			for (int i = pat.size() - 1; i >= patFrom; --i)
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
					--subjTo;
				}
			}
			if (subjFrom >= subjTo)
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
		int subjStart = -1;
		int subjEnd = -1;
		if (pat.size() > subj.size())
		{
			return false;
		}
		{
			//compare left border
			int subjI = 0;
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
						subjStart = subjI;
						break;
					}
				}
				else if (!compare(pat[patI].get(), subj[subjI].get()))
				{
					return false;
				}
				++subjI;
			}
			if (subjStart == -1)
			{
				//there are no variables in pat
				//comparisson succeded at that level
				return true;
			}
		}
		{
			//compare right border
			int subjI = subj.size() - 1;
			for (int patI = pat.size() - 1; patI >= 0; --patI)
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
					const auto& captured = pat[patI]->variableMeta->captured;
					if (!captured.empty())
					{
						for (int capI = captured.size() - 1; capI >= 0; --capI)
						{
							if (!compare(captured[capI].get(), subj[subjI].get()))
							{
								return false;
							}
							--subjI;
						}
					}
					else
					{
						//found block start
						subjEnd = subjI + 1;
						break;
					}
				}
				else if (!compare(pat[patI].get(), subj[subjI].get()))
				{
					return false;
				}
				--subjI;
			}
			if (subjEnd <= subjStart)
			{
				return false;
			}
		}
		Block* block = level.findBlock(pat.back()->parent);
		assert(block);
		if (block->pat.size() > subjEnd - subjStart)
		{
			return false;
		}
		block->subj = std::span(subj).subspan(subjStart, subjEnd - subjStart);
		return true;
	}

	void setupDiophantineRhs(Bundle& bundle)
	{
		Equation* eq = &bundle.eq;
		for (auto* block : bundle.blocks)
		{
			eq->rhs = block->subj.size() - (block->pat.size() - block->vars.size());
			eq = eq->next.get();
		}
	}

	bool initVariables(Bundle& bundle)
	{
		for (auto& block : bundle.blocks)
		{
			auto& pat = block->pat;
			auto& subj = block->subj;
			int subjI = 0;
			for (int patI = 0; patI < block->pat.size(); ++patI)
			{
				if (pat[patI]->isPattern)
				{
					if (pat[patI]->label == subj[subjI]->label)
					{
						pat[patI]->subj = subj[subjI].get();
					}
					else
					{
						return false;
					}
					++subjI;
				}
				else if (pat[patI]->isVariable)
				{
					if (pat[patI]->variableMeta->captured.empty())
					{
						int varSize = pat[patI]->variableMeta->captureSizeNondet;
						pat[patI]->variableMeta->captured =
							std::span(subj).subspan(subjI, varSize);
						subjI += varSize;
					}
					else
					{
						auto& captured = pat[patI]->variableMeta->captured;
						for (int varI = 0; varI < captured.size();++varI)
						{
							if (!compare(captured[varI].get(), subj[subjI].get()))
							{
								return false;
							}
							++subjI;
						}
					}
				}
				else
				{
					if (!compare(pat[patI].get(), subj[subjI].get()))
					{
						return false;
					}
					++subjI;
				}

			}
		}
		return true;
	}

	Status solve(int pos, Equation& eq, int remainder)
	{
		if (pos == eq.initVars.size())
		{
			if (remainder == 0)
			{
				//add variables were subtracted from rhs and it became zero
				//means that we found a solution
				{
					//print solution
					int sum = 0;
					for (auto val : eq.initVars)
					{
						sum += val.var->captureSizeNondet * val.coef;
						std::cout << val.var->captureSizeNondet << " ";
					}
					std::cout << "    " << sum;
					std::cout << "\n";

				}
				if (eq.next)
				{
					//has next equation
					//subtract variables initialized by previous equations
					int sum = 0;
					for (auto& var : eq.next->prevVars)
					{
						sum += var.var->captureSizeNondet * var.coef;
					}
					solve(0, *eq.next.get(), eq.next->rhs - sum);
				}
				else
				{
					//continue comparison

				}
				return Status::succeeded;
			}
			else if (remainder > 0)
			{
				// haven't found solution yet, try next iteration
				return Status::succeeded;
			}
		}

		if (remainder <= 0)
		{
			return Status::exceeded;
		}

		for (int i = 1; ; ++i)
		{
			eq.initVars[pos].var->captureSizeNondet = i;
			auto status = solve(pos + 1, eq, remainder - (i * eq.initVars[pos].coef));
			if (status == Status::finished)
			{
				return Status::finished;
			}
			else if (status == Status::exceeded)
			{
				break;
			}
		}
		return Status::succeeded;
	}

	bool comparissonTest(std::vector<Level>& levels, std::vector<std::unique_ptr<TermTest>>& pat, std::vector<std::unique_ptr<TermTest>>& subj, int levelI = 0)
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
		if (!compare1(level, pat, subj))
		{
			return false;
		}
		//chose variation
		for (auto& bundle : level.bundles)
		{
			if (!bundle.blocks.empty())
			{
				setupDiophantineRhs(bundle);
				solve(0, bundle.eq, bundle.eq.rhs);
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
		auto subjStr = L"t(f(a,b,`x11,`x12,k(`d1,`d2,`d3,`k1),`x1,`x2,`x3,`x4,t),f1(`t1,`t2,`k1),d(`b1,d(a,`t1,`t2,`k11,`k21),`x11,`x12))";
		Parser subjParser(subjStr);
		subjParser.parse();
		std::unique_ptr<TermTest> subjTerm = std::unique_ptr<TermTest>(subjParser.m_current_term);
		std::vector<std::unique_ptr<TermTest>> subj;
		markVariables(subjTerm);
		subj.push_back(std::move(subjTerm));

		auto patStr = L"t(f(a,b,`x1,k(`d,`k),`x,t),f1(`t,`k),d(`b,d(a,`t,`k1,`k2),`x1))";
		Parser patParser(patStr);
		patParser.parse();
		std::unique_ptr<TermTest> patTerm = std::unique_ptr<TermTest>(patParser.m_current_term);
		std::vector<std::unique_ptr<TermTest>> pat;
		markVariables(patTerm);
		pat.push_back(std::move(patTerm));
		enumerate(pat);
		std::vector<TermTest*> vars;
		unifyVariables(pat, vars);
		markPatternNodes(pat);
		auto levels = generateLevels(pat);

		comparissonTest(levels, pat, subj);
		/*std::wcout << patStr << "\n";
		for (auto& lev : levels)
		{
			std::cout << "\n=========level========\n";
			std::cout << "===bundles\n";
			for (auto& bundle : lev.bundles)
			{
				{
					TestFramework::ColorGuard green(TestFramework::GREEN);
					std::cout << "[";
				}
				for (auto& block : bundle.blocks)
				{
					{
						TestFramework::ColorGuard red(TestFramework::RED);
						std::cout << "{";
					}
					print(block->pat);
					{
						TestFramework::ColorGuard red(TestFramework::RED);
						std::cout << "} ";
					}
				}
				{
					TestFramework::ColorGuard green(TestFramework::GREEN);
					std::cout << "] ";
				}
			}
			std::cout << "\n===determined\n";
			for (auto& path : lev.determinedVars)
			{
				std::cout << "[";
				for (auto& pos : path)
				{
					std::cout << pos.pos << "(" << (pos.fromLeft ? "L" : "R") << ")";
				}
				std::cout << "]\n";
			}
		}*/


	}
}