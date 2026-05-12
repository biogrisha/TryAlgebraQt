#pragma once
#include <map>
#include <string>
#include <functional>
#include <Me/include/MeBase.h>
#include <MathEditor/include/TextBuffer.h>

namespace TryAlgebraCore2
{
	class MeContainer;
	class MeGenerator
	{
	public:
		std::unique_ptr<MeBase> generateMe(const std::wstring& me_str);
	private:
		std::map<std::string, std::function<MeBase*>> m_generators_map;
	};

	class MeParser
	{
	public:
		MeParser(const TextBuffer& text_buffer, int line_num);
		bool parseLine(MeContainer* container);
		void parse(bool parse_one_line);
		void consumeMe();
		void consumeMeta();
		void startChildren();
	private:
		TextBufferIterator m_it;
		std::wstring line_str;
		MeBase* m_current = nullptr;
		MeBase* m_parent = nullptr;
		std::vector<std::unique_ptr<MeBase>> generated_me;
		int depth = -1;
	};
}