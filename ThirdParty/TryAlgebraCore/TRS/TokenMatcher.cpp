#include "TokenMatcher.h"

namespace TryAlgebraCore
{

	int TokenMatcher::Match::startCharIndex(const std::vector<std::wstring>& tokens) const
	{
		return endCharIndex
			- static_cast<int>(tokens[tokenIndex].size())
			+ 1;
	}

	TokenMatcher::TokenMatcher(std::vector<std::wstring> tokens)
		: m_tokens(std::move(tokens))
	{
		m_nodes.emplace_back(); // root

		buildTrie();
		buildFailureLinks();
	}

	std::optional<TokenMatcher::Match> TokenMatcher::findNext(const std::wstring& string, int& pos) const
	{
		size_t state = 0;

		while (pos < string.size())
		{
			wchar_t ch = string[pos];
			++pos;

			// Follow failure links until we either find a transition
			// or reach the root.
			while (state != 0 &&
				m_nodes[state].next.find(ch) == m_nodes[state].next.end())
			{
				state = m_nodes[state].failure;
			}

			auto transition = m_nodes[state].next.find(ch);

			if (transition != m_nodes[state].next.end())
				state = transition->second;
			else
				state = 0;

			if (!m_nodes[state].outputs.empty())
			{
				// If several tokens finish at this character,
				// return the first one.
				//
				// You could instead select longest/shortest here.
				return Match{
					m_nodes[state].outputs.front(),
					pos
				};
			}
		}

		return std::nullopt;
	}

	void TokenMatcher::buildTrie()
	{
		for (size_t tokenIndex = 0; tokenIndex < m_tokens.size(); ++tokenIndex)
		{
			const auto& token = m_tokens[tokenIndex];

			if (token.empty())
				continue;

			size_t state = 0;

			for (wchar_t ch : token)
			{
				auto& transitions = m_nodes[state].next;

				auto it = transitions.find(ch);

				if (it != transitions.end())
				{
					state = it->second;
				}
				else
				{
					const size_t newState = m_nodes.size();

					// Do this while nodes_[state] is definitely valid.
					transitions.emplace(ch, newState);

					// This may reallocate nodes_.
					m_nodes.emplace_back();

					// Don't use `transitions` or `it` after emplace_back().
					state = newState;
				}
			}

			m_nodes[state].outputs.push_back(tokenIndex);
		}
	}

	void TokenMatcher::buildFailureLinks()
	{
		std::queue<size_t> queue;

		// Children of root fail back to root.
		for (const auto& [ch, child] : m_nodes[0].next)
		{
			m_nodes[child].failure = 0;
			queue.push(child);
		}

		while (!queue.empty())
		{
			size_t state = queue.front();
			queue.pop();

			for (const auto& [ch, child] : m_nodes[state].next)
			{
				size_t failure = m_nodes[state].failure;

				while (failure != 0 &&
					m_nodes[failure].next.find(ch) ==
					m_nodes[failure].next.end())
				{
					failure = m_nodes[failure].failure;
				}

				auto it = m_nodes[failure].next.find(ch);

				if (it != m_nodes[failure].next.end())
					m_nodes[child].failure = it->second;
				else
					m_nodes[child].failure = 0;

				// A suffix may itself be a complete token.
				const auto& inherited =
					m_nodes[m_nodes[child].failure].outputs;

				m_nodes[child].outputs.insert(
					m_nodes[child].outputs.end(),
					inherited.begin(),
					inherited.end());

				queue.push(child);
			}
		}
	}
	const std::vector<std::wstring>& TokenMatcher::tokens() const
	{
		return m_tokens;
	}
}
