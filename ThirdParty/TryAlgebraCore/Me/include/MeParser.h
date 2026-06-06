#pragma once
#include <unordered_map>
#include <string>
#include <functional>
#include <Me/include/MeBase.h>
#include <MathEditor/include/TextBuffer.h>

namespace TryAlgebraCore
{
	class MeContainer;

	class MeParser
	{
		enum class ParsingResult
		{
			next_child,
			end_children,
			end_line,
			end,
			none
		};
	public:
		MeParser(const TextBuffer& text_buffer, int line_num);
		bool parseLine(MeBase* container);
	private:
		ParsingResult parse(bool parse_one_line);
		void consumeMe();
		void consumeMeta();
		void startChildren();
		std::unique_ptr<MeBase> make(const std::wstring& name);

		TextBufferIterator m_it;
		MeBase* m_current = nullptr;
		MeBase* m_parent = nullptr;
		std::unordered_map<std::wstring, std::function<std::unique_ptr<MeBase>()>> m_factory;
		int depth = -1;
	};
}