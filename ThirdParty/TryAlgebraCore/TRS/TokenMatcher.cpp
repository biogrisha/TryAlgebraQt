#include "TokenMatcher.h"

namespace TryAlgebraCore
{

	int TokenMatcher::Match::startCharIndex(const std::vector<std::wstring>& tokens) const
	{
		return endCharIndex
			- static_cast<int>(tokens[tokenIndex].size())
			+ 1;
	}

	TokenMatcher::TokenMatcher(const std::vector<std::wstring>& tokens)
		: tokens_(tokens)
	{
		nodes_.emplace_back(); // root

		buildTrie();
		buildFailureLinks();
	}

	std::optional<TokenMatcher::Match> TokenMatcher::findNext(TextBufferIterator& it) const
	{
		size_t state = 0;

		while (!it.isEnd())
		{
			wchar_t ch = it.next();

			// Follow failure links until we either find a transition
			// or reach the root.
			while (state != 0 &&
				nodes_[state].next.find(ch) == nodes_[state].next.end())
			{
				state = nodes_[state].failure;
			}

			auto transition = nodes_[state].next.find(ch);

			if (transition != nodes_[state].next.end())
				state = transition->second;
			else
				state = 0;

			if (!nodes_[state].outputs.empty())
			{
				// If several tokens finish at this character,
				// return the first one.
				//
				// You could instead select longest/shortest here.
				return Match{
					nodes_[state].outputs.front(),
					it.getChId()
				};
			}
		}

		return std::nullopt;
	}

	void TokenMatcher::buildTrie()
	{
		for (size_t tokenIndex = 0; tokenIndex < tokens_.size(); ++tokenIndex)
		{
			const auto& token = tokens_[tokenIndex];

			if (token.empty())
				continue;

			size_t state = 0;

			for (wchar_t ch : token)
			{
				auto& transitions = nodes_[state].next;

				auto it = transitions.find(ch);

				if (it != transitions.end())
				{
					state = it->second;
				}
				else
				{
					const size_t newState = nodes_.size();

					// Do this while nodes_[state] is definitely valid.
					transitions.emplace(ch, newState);

					// This may reallocate nodes_.
					nodes_.emplace_back();

					// Don't use `transitions` or `it` after emplace_back().
					state = newState;
				}
			}

			nodes_[state].outputs.push_back(tokenIndex);
		}
	}

	void TokenMatcher::buildFailureLinks()
	{
		std::queue<size_t> queue;

		// Children of root fail back to root.
		for (const auto& [ch, child] : nodes_[0].next)
		{
			nodes_[child].failure = 0;
			queue.push(child);
		}

		while (!queue.empty())
		{
			size_t state = queue.front();
			queue.pop();

			for (const auto& [ch, child] : nodes_[state].next)
			{
				size_t failure = nodes_[state].failure;

				while (failure != 0 &&
					nodes_[failure].next.find(ch) ==
					nodes_[failure].next.end())
				{
					failure = nodes_[failure].failure;
				}

				auto it = nodes_[failure].next.find(ch);

				if (it != nodes_[failure].next.end())
					nodes_[child].failure = it->second;
				else
					nodes_[child].failure = 0;

				// A suffix may itself be a complete token.
				const auto& inherited =
					nodes_[nodes_[child].failure].outputs;

				nodes_[child].outputs.insert(
					nodes_[child].outputs.end(),
					inherited.begin(),
					inherited.end());

				queue.push(child);
			}
		}
	}
}
