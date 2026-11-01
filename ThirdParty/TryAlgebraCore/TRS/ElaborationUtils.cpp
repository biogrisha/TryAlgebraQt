#include "ElaborationUtils.h"
#include "ElaborationUtils.h"
#include <unordered_map>
#include <iterator>
namespace TryAlgebraCore::Trs::ElaborationUtils
{
	namespace {
		std::unordered_map<std::wstring, std::wstring> brackets =
		{
			{L")", L"("},
			{L"]", L"["},
		};

		constexpr const wchar_t* argLabel = L"arg";
		constexpr const wchar_t* bracketLabel = L"br";

		void elaborateBrackets(std::vector<std::unique_ptr<TermIntermediate>>& subj)
		{
			for (auto& t : subj)
			{
				elaborateBrackets(t->children);
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
						term->parent = subj.back()->parent;
						term->label = bracketLabel;
						std::move(subj.begin() + i + 1, subj.begin() + intervals.back().to
							, std::back_inserter(term->children));
						subj.erase(subj.begin() + i, subj.begin() + intervals.back().to + 1);
						for (auto& ch : term->children)
						{
							ch->parent = term.get();
						}
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

		void removeBrackets(std::vector<std::unique_ptr<TermIntermediate>>& subj)
		{
			//TODO: parent not updated
			//for (int i = 0; i < subj.size(); ++i)
			//{
			//	removeBrackets(subj[i]->children);
			//	if (subj[i]->label == bracketLabel)
			//	{
			//		subj.insert(
			//			subj.begin() + i + 1,
			//			std::make_move_iterator(subj[i]->children.begin()),
			//			std::make_move_iterator(subj[i]->children.end())
			//		);
			//		subj.erase(subj.begin() + i);
			//	}
			//}
		}

		void elaborateArgs(std::vector<std::unique_ptr<TermIntermediate>>& subj, TermIntermediate* parent = nullptr)
		{
			for (auto& t : subj)
			{
				elaborateArgs(t->children, t.get());
			}
			if (!parent || parent->label != bracketLabel)
			{
				return;
			}
			std::vector<std::unique_ptr<TermIntermediate>> res;
			int from = 0;
			auto insertTerms = [&res, &subj](int from, int to)
				{
					if (from >= to)
					{
						return;
					}
					auto arg = std::make_unique<TermIntermediate>();
					arg->label = argLabel;
					arg->parent = subj[from]->parent;
					for (int i = from; i < to; ++i)
					{
						subj[i]->parent = arg.get();
						arg->children.push_back(std::move(subj[i]));
					}
					res.push_back(std::move(arg));
				};
			for (int i = 0; i < subj.size(); ++i)
			{
				if (subj[i]->label == L",")
				{
					insertTerms(from, i);
					from = i + 1;
				}
			}
			insertTerms(from, subj.size());
			subj = std::move(res);
		}

		void appendTermsToString(
			const std::span<std::unique_ptr<TryAlgebraCore::Trs::TermIntermediate>>& terms,
			std::wstring& result)
		{
			for (std::size_t i = 0; i < terms.size(); ++i)
			{
				const auto& term = terms[i];
				result += term->label;
				if (!term->children.empty())
				{
					result += L'(';
					appendTermsToString(term->children, result);
					result += L')';
				}
				if (i + 1 < terms.size())
				{
					result += L',';
				}
			}
		}

	}

	void commonTermElaboration(std::vector<std::unique_ptr<TermIntermediate>>& terms)
	{
		elaborateBrackets(terms);
		elaborateArgs(terms);
	}
	std::wstring TryAlgebraCore::Trs::ElaborationUtils::termsToString(const std::span<std::unique_ptr<TermIntermediate>>& terms)
	{
		std::wstring result;
		appendTermsToString(terms, result);
		return result;
	}
}
