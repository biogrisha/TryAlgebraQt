#pragma once

#include "TestHelpers.h"
#include "TestGlobals.h"
#include <iostream>
#include <span>
#include <unordered_map>
#include <functional>
namespace PatternMatchingTest
{
	struct TermTest;
	struct VariableMeta
	{
		std::span<std::unique_ptr<TermTest>> captured;
		bool isCaptured = false;
		int captureSizeNondet = 0;
		enum class Type
		{
			ZeroMulti,
			OneMulti,
			Uni,
		} type = Type::OneMulti;
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
		exceeded,
		succeeded,
		solutionFound
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
		bool isPureVar()
		{
			return isVariable
				&& variableMeta.get() != nullptr
				&& !variableMeta->isCaptured;
		}
	};

	struct Block
	{
		int minPatCapture = 0;
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
		static Block* findBlock(const std::vector<Bundle*>& siblings, const TermTest* parent)
		{
			for (auto* bundle : siblings)
			{
				for (auto* block : bundle->blocks)
				{
					if (block->pat.back()->parent == parent)
					{
						return block;
					}
				}
			}
			return nullptr;
		}
		std::vector<Block*> blocks;
		Equation eq;
		std::vector<Bundle*> children;
	};

	struct Level
	{
		std::vector<Block> blocks;
		std::vector<Bundle> bundles;
	};

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

	inline void collectBlocks(std::vector<std::unique_ptr<TermTest>>& pat, std::vector<Block>& blocks)
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
					collectBlocks(el->children, blocks);
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

		//this sequence has variables
		//create block for them
		Block block;
		block.pat = std::span(pat).subspan(varStart, varEnd - varStart);
		for (auto& var : block.pat)
		{
			if (var->isVariable && var->variableMeta->type == VariableMeta::Type::ZeroMulti)
			{
				continue;
			}
			++block.minPatCapture;
		}
		blocks.push_back(block);

		//recursive call for all patterns on the left and on the right
		for (int i = 0; i < varStart; ++i)
		{
			if (pat[i]->isPattern)
			{
				collectBlocks(pat[i]->children, blocks);
			}
		}
		for (int i = varEnd; i < pat.size(); ++i)
		{
			if (pat[i]->isPattern)
			{
				collectBlocks(pat[i]->children, blocks);
			}
		}
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

	bool bundlesRelated(Bundle& b1, Bundle& b2)
	{

		for (auto* bl1 : b1.blocks)
		{
			for (auto* v1 : bl1->varsRec)
			{
				for (auto* bl2 : b2.blocks)
				{
					for (auto* v2 : bl2->varsRec)
					{
						if (compare(v1, v2))
						{
							return true;
						}
					}
				}
			}
		}

		return false;
	}
	void setupBundlesTree(std::vector<Level>& levels)
	{
		for (int levI = 0; levI < levels.size() - 1; ++levI)
		{
			for (int bundI = 0; bundI < levels[levI].bundles.size(); ++bundI)
			{
				for (int chBundI = 0; chBundI < levels[levI + 1].bundles.size(); ++chBundI)
				{
					if (bundlesRelated(levels[levI].bundles[bundI], levels[levI + 1].bundles[chBundI]))
					{
						levels[levI].bundles[bundI].children.push_back(&levels[levI + 1].bundles[chBundI]);
					}
				}

			}
		}

	}
	inline std::vector<Level> generateLevels(std::vector<std::unique_ptr<TermTest>>& pat)
	{
		std::vector<Level> levels;
		{
			std::vector<Block> blocks;
			collectBlocks(pat, blocks);
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
			levels.emplace_back(std::move(blocks));
		}

		while (true)
		{
			std::vector<Block> blocks;
			for (auto& bl : levels.back().blocks)
			{
				for (auto& el : bl.pat)
				{
					if (el->isPattern)
					{
						collectBlocks(el->children, blocks);
					}
				}
			}
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
				break;
			}
			levels.emplace_back(std::move(blocks));
		}

		collectBundles(levels);
		setupBundlesTree(levels);
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

	bool compare1(std::vector<Bundle*>& childBundles, std::vector<std::unique_ptr<TermTest>>& pat, std::vector<std::unique_ptr<TermTest>>& subj)
	{
		int subjStart = -1;
		int subjEnd = -1;
		//if (pat.size() > subj.size())
		//{
		//	return false;
		//}
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
					if (!compare1(childBundles, pat[patI]->children, subj[subjI]->children))
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
				else if (patI >= pat.size() || subjI >= subj.size() || !compare(pat[patI].get(), subj[subjI].get()))
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
					if (!compare1(childBundles, pat[patI]->children, subj[subjI]->children))
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
						continue;
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
		}
		Block* block = Bundle::findBlock(childBundles, pat.back()->parent);
		assert(block);
		if (block->minPatCapture > subjEnd - subjStart)
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
						for (int varI = 0; varI < captured.size(); ++varI)
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

	Status solve(int pos, Equation& eq, int remainder, std::function<bool()> callback)
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
					auto res = solve(0, *eq.next.get(), eq.next->rhs - sum, callback);
					if (res == Status::solutionFound)
					{
						return res;
					}
				}
				else if (callback())
				{
					//continue comparison
					return Status::solutionFound;
				}
				return Status::succeeded;
			}
			else if (remainder > 0)
			{
				// haven't found solution yet, try next iteration
				return Status::succeeded;
			}
		}

		if (remainder < 0 || remainder == 0 && eq.initVars[pos].var->type != VariableMeta::Type::ZeroMulti)
		{
			return Status::exceeded;
		}

		for (int i = eq.initVars[pos].var->type == VariableMeta::Type::ZeroMulti ? 0 : 1;
			eq.initVars[pos].var->type != VariableMeta::Type::Uni || i < 2; ++i)
		{
			eq.initVars[pos].var->captureSizeNondet = i;
			auto status = solve(pos + 1, eq, remainder - (i * eq.initVars[pos].coef), callback);
			if (status == Status::exceeded)
			{
				break;
			}
			else if (status == Status::solutionFound)
			{
				return Status::solutionFound;
			}
		}
		return Status::succeeded;
	}

	bool comparissonTest(std::vector<Level>& levels, std::vector<std::unique_ptr<TermTest>>& pat, std::vector<std::unique_ptr<TermTest>>& subj, int levelI = 0)
	{
		auto& level = levels[levelI];

		for (auto& bundle : level.bundles)
		{
			if (!bundle.blocks.empty())
			{
				setupDiophantineRhs(bundle);
				solve(0, bundle.eq, bundle.eq.rhs, []()
					{
						std::cout << "solution found";
						return true;
					}
				);
				break;
			}
		}
		return false;
	}

	bool compare3(std::vector<Bundle*>& childBundles)
	{
		//bundles subjects collected
		for (auto* bundle : childBundles)
		{
			//form diophantine
			setupDiophantineRhs(*bundle);
			auto res = solve(0, bundle->eq, bundle->eq.rhs, [bundle]()
				{
					//found solution of equation
					//assign subjects to variables and p-terms
					for (auto* block : bundle->blocks)
					{
						for (auto* var : block->vars)
						{
							var->variableMeta->captured = {};
						}
					}
					if (!initVariables(*bundle))
					{
						return false;
					}
					//compare and initialize blocks in bundle
					for (auto* block : bundle->blocks)
					{
						for (auto& t : block->pat)
						{
							if (t->isPattern)
							{
								if (!compare1(bundle->children, t->children, t->subj->children))
								{
									return false;
								}
							}
						}
					}
					return compare3(bundle->children);
				});
			if (res != Status::solutionFound)
			{
				return false;
			}
		}
		return true;
	}

	bool compare2(std::vector<Bundle*>& childBundles, std::vector<std::unique_ptr<TermTest>>& pat, std::vector<std::unique_ptr<TermTest>>& subj)
	{
		//compare everithing we can and collect bundles subjects
		if (!compare1(childBundles, pat, subj))
		{
			return false;
		}
		return compare3(childBundles);
	}

	void markVariables(const std::unique_ptr<TermTest>& term)
	{
		if (term->label[0] == L'`')
		{
			term->isVariable = true;
			term->variableMeta = std::make_shared<VariableMeta>();
			term->variableMeta->type = VariableMeta::Type::OneMulti;
			return;
		}
		else if (term->label[0] == L'~')
		{
			term->isVariable = true;
			term->variableMeta = std::make_shared<VariableMeta>();
			term->variableMeta->type = VariableMeta::Type::Uni;
			return;
		}
		else if (term->label[0] == L'_')
		{
			term->isVariable = true;
			term->variableMeta = std::make_shared<VariableMeta>();
			term->variableMeta->type = VariableMeta::Type::ZeroMulti;
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

	void printBundles(Bundle* b, int depth = 0)
	{
		std::string offset;
		for (int i = 0; i < depth; ++i)
		{
			offset += " ";
		}
		{
			TestFramework::ColorGuard green(TestFramework::GREEN);
			std::cout << offset << "[";
		}
		for (auto& block : b->blocks)
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
			std::cout << "] \n";
		}
		for (auto* ch : b->children)
		{
			printBundles(ch, depth + 1);
		}
	}
	MYTEST(VariatorTest)
	{
		auto subjStr = L"t(h)";
		Parser subjParser(subjStr);
		subjParser.parse();
		std::unique_ptr<TermTest> subjTerm = std::unique_ptr<TermTest>(subjParser.m_current_term);
		std::vector<std::unique_ptr<TermTest>> subj;
		markVariables(subjTerm);
		subj.push_back(std::move(subjTerm));

		auto patStr = L"t(h,t,_h,f)";
		Parser patParser(patStr);
		patParser.parse();
		std::unique_ptr<TermTest> patTerm = std::unique_ptr<TermTest>(patParser.m_current_term);
		std::vector<std::unique_ptr<TermTest>> pat;
		markVariables(patTerm);
		pat.push_back(std::move(patTerm));
		std::vector<TermTest*> vars;
		unifyVariables(pat, vars);
		markPatternNodes(pat);
		auto levels = generateLevels(pat);
		std::vector<Bundle*> bundles;
		for (auto& b : levels.front().bundles)
		{
			bundles.push_back(&b);
		}
		if (compare2(bundles, pat, subj))
		{
			std::cout << "SUCCESS \n";
			for (auto var : vars)
			{
				std::wcout << L"\n" << var->label << L"{";
				print(var->variableMeta->captured);
				std::cout << "}";
			}
		}

		std::wcout << "\n";
		std::wcout << patStr << "\n";
		std::cout << "===bundles\n";
		for (auto& bundle : levels[0].bundles)
		{
			printBundles(&bundle);
		}



	}
}