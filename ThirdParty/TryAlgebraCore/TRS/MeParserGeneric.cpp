#include "MeParserGeneric.h"

namespace TryAlgebraCore::Trs {

	MeParserGeneric::MeParserGeneric(const std::wstring& str)
		:m_str(str)
	{

	}

	void MeParserGeneric::parse()
	{
		State state = State::ReadGlyph;
		std::wstring_view str;
		int readStart = 0;
		bool finishedWithEndChildren = false;
		for (int i = 0; i < static_cast<int>(m_str.size()); ++i)
		{
			if (i < m_str.size() - 1)
			{
				if (m_str[i] == L'\\' && m_str[i + 1] == L'{')
				{
					if (state == State::ReadMeta)
					{
						addMeta(str);
					}
					else if (state == State::ReadName)
					{
						createMe(str);
					}
					state = State::ReadGlyph;
					++i;
					startChildren();
					continue;
				}
				else if (m_str[i] == L'\\' && m_str[i + 1] == L',')
				{
					++i;
					nextChild();
					continue;
				}
				else if (m_str[i] == L'\\' && m_str[i + 1] == L'}')
				{
					++i;
					endChildren();
					continue;
				}
				else if (m_str[i] == L'\\')
				{
					readStart = i + 1;
					if (state == State::ReadGlyph)
					{
						state = State::ReadName;
					}
					else if (state == State::ReadName)
					{
						state = State::ReadMeta;
						createMe(str);
					}
					else if (state == State::ReadMeta)
					{
						state = State::ReadGlyph;
						addMeta(str);
					}
					continue;
				}
			}
			if (state == State::ReadName || state == State::ReadMeta)
			{
				str = std::wstring_view{ m_str }.substr(readStart, i - readStart + 1);
			}
			else if (state == State::ReadGlyph)
			{
				addGlyph(m_str[i]);
			}
		}
		if (state == State::ReadName)
		{
			createMe(str);
		}
		else if (state == State::ReadMeta)
		{
			addMeta(str);
		}
	}
}
