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
	};

	struct TermTest
	{
		std::wstring label;
		std::vector<std::unique_ptr<TermTest>> children;
		bool isVariable = false;
		bool isPattern = false;
		std::shared_ptr<VariableMeta> variableMeta;
	};

	struct Block
	{
		std::span<std::unique_ptr<TermTest>> terms;
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

	void unifyVariables(std::vector<std::unique_ptr<TermTest>>& terms, std::vector<TermTest*>& variables)
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

	void collectBlocks(std::vector<std::unique_ptr<TermTest>>& pat, std::vector<Block>& blocks)
	{
		int varStart = -1;
		int varEnd = -1;
		//find varStart
		for (int i = 0; i < pat.size(); ++i)
		{
			if (pat[i]->isVariable && !pat[i]->variableMeta->isDetermined)
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
		// find var end (last var pos + i)
		for (int i = pat.size() - 1; i >= varStart; --i)
		{
			if (pat[i]->isVariable && !pat[i]->variableMeta->isDetermined)
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
			for (auto& el : pat)
			{
				if (el->isPattern)
				{
					collectBlocks(el->children, blocks);
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

	bool removeDetermined(std::vector<Block>& blocks)
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
				if (el->isVariable && !el->variableMeta->isDetermined)
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
						collectBlocks(el->children, blocks);
					}
				}
				std::swap(blocks[blI], blocks.back());
				blocks.pop_back();
				continue;
			}
			for (int i = block.terms.size() - 1; i >= varStart; --i)
			{
				auto& el = block.terms[i];
				if (el->isVariable && !el->variableMeta->isDetermined)
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
						collectBlocks(el->children, blocks);
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
						collectBlocks(block.terms[i]->children, blocks);
					}
				}
				for (int i = varEnd; i < block.terms.size(); ++i)
				{
					if (block.terms[i]->isPattern)
					{
						collectBlocks(block.terms[i]->children, blocks);
					}
				}
				block.terms.subspan(varStart, varEnd - varStart);
			}

		}
		return res;
	}

	void func(std::vector<std::unique_ptr<TermTest>>& pat)
	{
		std::vector<std::vector<Block>> levels;
		{
			std::vector<Block> blocks;
			collectBlocks(pat, blocks);
			while (removeDetermined(blocks));
			levels.push_back(std::move(blocks));
		}

		while (true)
		{
			std::vector<Block> blocks;
			for (auto& bl : levels.back())
			{
				for (auto& el : bl.terms)
				{
					if (el->isPattern)
					{
						collectBlocks(el->children, blocks);
					}
				}
			}
			while (removeDetermined(blocks));
			if (blocks.empty())
			{
				break;
			}
			levels.push_back(std::move(blocks));
		}

	}

	MYTEST(VariatorTest)
	{
		std::vector<int> ints = { 1,2,3,4,5,6 };
		std::span sp(ints);
		auto ssp = sp.subspan(2);
		auto sssp = ssp.subspan(0, 1);
		for (auto el : sssp)
		{
			std::cout << el;
		}
	}
}