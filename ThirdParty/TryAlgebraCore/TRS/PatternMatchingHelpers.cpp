#include "PatternMatchingHelpers.h"
#include <Me/include/MeGlobals.h>
#include <TRS/MeParserGeneric.h>

namespace TryAlgebraCore::Trs
{
	Block* Bundle::findBlock(const std::vector<Bundle*>& siblings, const TermIntermediate* parent)
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

	bool compare(TermIntermediate* lhs, TermIntermediate* rhs)
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

	void unifyVariables(std::vector<std::unique_ptr<TermIntermediate>>& terms, std::vector<TermIntermediate*>& variables)
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

	bool markPatternNodes(std::vector<std::unique_ptr<TermIntermediate>>& t)
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

	void collectBlocks(std::vector<std::unique_ptr<TermIntermediate>>& pat, std::vector<Block>& blocks)
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

	void collectVariablesRec(const std::span<std::unique_ptr<TermIntermediate>>& block, std::unordered_set<TermIntermediate*>& vars)
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

	void initDiophantineEq(Bundle& bundle)
	{
		std::unordered_set<VariableMeta*> vars;
		DioEquation* eq = &bundle.eq;
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
				eq->next = std::make_unique<DioEquation>();
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

	std::vector<Level> generateLevels(std::vector<std::unique_ptr<TermIntermediate>>& pat)
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

	bool compare1(std::vector<Bundle*>& childBundles, std::vector<std::unique_ptr<TermIntermediate>>& pat, std::vector<std::unique_ptr<TermIntermediate>>& subj)
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
			if (subjEnd <= subjStart)
			{
				return false;
			}
		}
		Block* block = Bundle::findBlock(childBundles, pat.back()->parent);
		if (block->pat.size() > subjEnd - subjStart)
		{
			return false;
		}
		block->subj = std::span(subj).subspan(subjStart, subjEnd - subjStart);
		return true;
	}

	void setupDiophantineRhs(Bundle& bundle)
	{
		DioEquation* eq = &bundle.eq;
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

	DioStatus solve(int pos, DioEquation& eq, int remainder, std::function<bool()> callback)
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
					}
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
					if (res == DioStatus::solutionFound)
					{
						return res;
					}
				}
				else if (callback())
				{
					//continue comparison
					return DioStatus::solutionFound;
				}
				return DioStatus::succeeded;
			}
			else if (remainder > 0)
			{
				// haven't found solution yet, try next iteration
				return DioStatus::succeeded;
			}
		}

		if (remainder <= 0)
		{
			return DioStatus::exceeded;
		}

		for (int i = 1; eq.initVars[pos].var->isMultiVariable || i < 2; ++i)
		{
			eq.initVars[pos].var->captureSizeNondet = i;
			auto status = solve(pos + 1, eq, remainder - (i * eq.initVars[pos].coef), callback);
			if (status == DioStatus::finished)
			{
				return DioStatus::finished;
			}
			else if (status == DioStatus::exceeded)
			{
				break;
			}
			else if (status == DioStatus::solutionFound)
			{
				return DioStatus::solutionFound;
			}
		}
		return DioStatus::succeeded;
	}

	bool comparissonTest(std::vector<Level>& levels, std::vector<std::unique_ptr<TermIntermediate>>& pat, std::vector<std::unique_ptr<TermIntermediate>>& subj, int levelI)
	{
		auto& level = levels[levelI];

		for (auto& bundle : level.bundles)
		{
			if (!bundle.blocks.empty())
			{
				setupDiophantineRhs(bundle);
				solve(0, bundle.eq, bundle.eq.rhs, []()
					{
						return true;
					}
				);
				break;
			}
		}
		return false;
	}

	bool compare2(std::vector<Bundle*>& childBundles)
	{
		//bundles subjects collected
		for (auto* bundle : childBundles)
		{
			//form diophantine
			setupDiophantineRhs(*bundle);
			auto res = solve(0, bundle->eq, bundle->eq.rhs, [bundle]()
				{
					for (auto* block : bundle->blocks)
					{
						for (auto* var : block->vars)
						{
							var->variableMeta->captured = {};
						}
					}
					//found solution of equation
					//assign subjects to variables and p-terms
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
					return compare2(bundle->children);
				});
			if (res != DioStatus::solutionFound)
			{
				return false;
			}
		}
		return true;
	}

	bool tryMatch(std::vector<Bundle*>& childBundles, std::vector<std::unique_ptr<TermIntermediate>>& pat, std::vector<std::unique_ptr<TermIntermediate>>& subj)
	{
		//compare everithing we can and collect bundles subjects
		if (!compare1(childBundles, pat, subj))
		{
			return false;
		}
		return compare2(childBundles);
	}

	void copyTermIntermediate(const std::span<std::unique_ptr<TermIntermediate>>& from
		, std::vector<std::unique_ptr<TermIntermediate>>& to
		, TermIntermediate* parent)
	{
		for (const auto& term : from)
		{
			auto& newItem = to.emplace_back(std::make_unique<TermIntermediate>());
			newItem->label = term->label;
			newItem->isPattern = term->isPattern;
			newItem->isVariable = term->isVariable;
			newItem->variableMeta = term->variableMeta;
			copyTermIntermediate(term->children, newItem->children, newItem.get());
		}
	}

	std::vector<std::unique_ptr<TermIntermediate>> copyTermIntermediate(const std::span<std::unique_ptr<TermIntermediate>>& from)
	{
		std::vector<std::unique_ptr<TermIntermediate>> res;
		copyTermIntermediate(from, res);
		return res;
	}

	void expandVars(std::vector<std::unique_ptr<TermIntermediate>>& term)
	{
		for (int i = term.size() - 1; i >= 0; --i)
		{
			if (term[i]->isVariable)
			{
				auto copy = copyTermIntermediate(term[i]->variableMeta->captured);
				term.erase(term.begin() + i);
				term.insert(term.begin() + i
					, std::make_move_iterator(copy.begin())
					, std::make_move_iterator(copy.end()));
			}
			else
			{
				expandVars(term[i]->children);
			}
		}
	}

	void markVariables(std::vector<std::unique_ptr<TermIntermediate>>& pat)
	{
		for (auto& t : pat)
		{
			if (t->label == MeNames::variable + MeNames::variableMulti)
			{
				t->isVariable = true;
				t->variableMeta = std::make_shared<VariableMeta>();
			}
			else if (t->label == MeNames::variable + MeNames::variableUni)
			{
				t->isVariable = true;
				t->variableMeta = std::make_shared<VariableMeta>();
				t->variableMeta->isMultiVariable = false;
			}
			else
			{
				markVariables(t->children);
			}
		}
	}

	std::vector<std::unique_ptr<TermIntermediate>> parseToTermIntermediate(const std::wstring& str)
	{
		Trs::MeParserGeneric parser(str);
		std::vector<std::unique_ptr<TermIntermediate>> res;
		auto* termsPtr = &res;
		TermIntermediate* parent = nullptr;
		TermIntermediate* lastTerm = nullptr;
		parser.createMe = [&termsPtr, &parent, &lastTerm](const std::wstring_view& str)
			{
				auto t = std::make_unique<TermIntermediate>();
				t->label = str;
				t->parent = parent;
				lastTerm = t.get();
				termsPtr->push_back(std::move(t));
			};
		parser.addMeta = [&lastTerm](const std::wstring_view& str)
			{
				lastTerm->label += str;
			};
		parser.addGlyph = [&termsPtr, &parent, &lastTerm](wchar_t g)
			{
				auto t = std::make_unique<TermIntermediate>();
				t->label = std::wstring(1, g);
				t->parent = parent;
				lastTerm = t.get();
				termsPtr->push_back(std::move(t));
			};
		parser.startChildren = [&termsPtr, &parent, &lastTerm]()
			{
				auto t = std::make_unique<TermIntermediate>();
				t->label = MeNames::cont;
				t->parent = lastTerm;
				termsPtr = &t->children;
				parent = t.get();
				lastTerm->children.push_back(std::move(t));
				lastTerm = nullptr;
			};
		parser.endChildren = [&termsPtr, &parent, &lastTerm, &terms = res]()
			{
				lastTerm = nullptr;
				parent = parent->parent->parent;

				termsPtr = parent ? &parent->children : &terms;
			};
		parser.nextChild = [&termsPtr, &parent, &lastTerm]()
			{
				auto t = std::make_unique<TermIntermediate>();
				t->label = MeNames::cont;
				t->parent = parent->parent;
				termsPtr = &t->children;
				parent = t.get();
				parent->parent->children.push_back(std::move(t));
				lastTerm = nullptr;
			};
		parser.parse();
		return res;
	}

}
