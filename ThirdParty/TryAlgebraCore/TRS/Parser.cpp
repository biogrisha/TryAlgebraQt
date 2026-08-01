#include "Parser.h"

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
		for (int i = 0; i < m_str.size() - 1; ++i)
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
			}
			else if (m_str[i] == L'\\' && m_str[i + 1] == L',')
			{
				++i;
				nextChild();
			}
			else if (m_str[i] == L'\\' && m_str[i + 1] == L'}')
			{
				++i;
				endChildren();
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
			}
			else if (state == State::ReadName || state == State::ReadMeta)
			{
				str = std::wstring_view{ m_str }.substr(readStart, i - readStart + 1);
			}
			else if (state == State::ReadGlyph)
			{
				addGlyph(m_str[i]);
			}

		}
		//last glyph
		str = std::wstring_view{ m_str }.substr(readStart, m_str.size());
		if (state == State::ReadMeta)
		{
			addMeta(str);
		}
		else if (state == State::ReadName)
		{
			createMe(str);
		}
		else if (state == State::ReadGlyph)
		{
			addGlyph(m_str.back());
		}

	}
}
