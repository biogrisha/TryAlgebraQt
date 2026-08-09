#include <string>
#include <vector>
#include <unordered_map>
#include <queue>
#include <optional>
#include <MathEditor/include/TextBuffer.h>

namespace TryAlgebraCore {

	class TokenMatcher
	{
	public:
		struct Match
		{
			size_t tokenIndex;
			int endCharIndex;

			// Inclusive start position.
			int startCharIndex(const std::vector<std::wstring>& tokens) const;
		};

		explicit TokenMatcher(const std::vector<std::wstring>& tokens);

		std::optional<Match> findNext(TextBufferIterator& it) const;

	private:
		struct Node
		{
			std::unordered_map<wchar_t, size_t> next;
			size_t failure = 0;

			// Indices into tokens_.
			std::vector<size_t> outputs;
		};

		void buildTrie();

		void buildFailureLinks();

	private:
		std::vector<std::wstring> tokens_;
		std::vector<Node> nodes_;
	};
}