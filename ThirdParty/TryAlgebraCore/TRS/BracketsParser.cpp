#include "BracketsParser.h"
#include <iterator>
namespace TryAlgebraCore::Trs
{
	namespace {
		std::unordered_map<std::wstring, std::wstring> brackets =
		{
			{L")", L"("},
			{L"]", L"["},
		};
	}
	void BracketsParser::apply(std::vector<std::unique_ptr<TermIntermediate>>& subj)
	{
		for (auto& t : subj)
		{
			apply(t->children);
		}

		struct Interval
		{
			int to = 0;
			std::wstring* opening = nullptr;
		};
		std::vector<Interval> intervals;
		for (int i = subj.size() - 1; i >= 0; --i)
		{
			if (!intervals.empty())
			{
				if (subj[i]->label == *intervals.back().opening)
				{
					//found opening bracket
					//create subterm and move elements into it
					auto term = std::make_unique<TermIntermediate>();
					term->label = L"br";
					std::move(subj.begin() + i + 1, subj.begin() + intervals.back().to
						, std::back_inserter(term->children));
					subj.erase(subj.begin() + i, subj.begin() + intervals.back().to + 1);
					intervals.pop_back();

					for (auto& interval : intervals)
					{
						interval.to -= term->children.size() + 1;
					}
					subj.insert(subj.begin() + i, std::move(term));
					continue;
				}
			}

			auto found = brackets.find(subj[i]->label);
			if (found != brackets.end())
			{
				auto& interval = intervals.emplace_back();
				//pos before bracket (need to delete brackets)
				interval.to = i;
				interval.opening = &found->second;
			}
		}
	}
}
