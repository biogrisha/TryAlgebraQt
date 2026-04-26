#pragma once
#include <map>
#include <string>
#include <functional>
#include <Me/include/MeBase.h>
#include <MathEditor/include/TextBuffer.h>

namespace TryAlgebraCore2
{
	class MeGenerator
	{
	public:
		std::unique_ptr<MeBase> generateMe(const std::wstring& me_str);
		std::unique_ptr<MeBase> generateMeChar(const std::wstring& str);
	private:
		std::map<std::string, std::function<MeBase*>> m_generators_map;
	};

	class MeParser
	{
	public:
		MeParser(const TextBuffer& text_buffer, int line_num);
		bool parseLine(std::vector<std::unique_ptr<MeBase>>& line);
	private:
		TextBufferIterator m_it;
	};
}