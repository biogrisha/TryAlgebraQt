#include <Me/include/MeParser.h>
#include <Me/include/MeCharacter.h>
namespace TryAlgebraCore2
{
	std::unique_ptr<MeBase> MeGenerator::generateMe(const std::wstring& me_str)
	{
		return std::unique_ptr<MeBase>();
	}
	std::unique_ptr<MeBase> MeGenerator::generateMeChar(const std::wstring& str)
	{
		std::unique_ptr<MeCharacter> me = std::make_unique<MeCharacter>(str);
		return me;
	}
	MeParser::MeParser(const TextBuffer& text_buffer, int line_num)
		:m_it(text_buffer, line_num)
	{

	}
	std::vector<std::unique_ptr<MeBase>> MeParser::parseLine()
	{
		std::vector<std::unique_ptr<MeBase>> line;
		std::wstring str;
		while (!m_it.isEnd() && !m_it.isNewLine())
		{
			str += m_it.next();
		}
		if(!str.empty())
		{
			line.push_back(MyRTTI::MakeTypedUnique<MeCharacter>(str));
		}
		return line;
	}
}