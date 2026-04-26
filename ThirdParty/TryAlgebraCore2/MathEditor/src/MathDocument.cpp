#include <MathEditor/include/MathDocument.h>
#include <Me/include/MeContainer.h>
#include <Me/include/MeParser.h>
#include <FreeTypeWrap.h>

namespace TryAlgebraCore2
{
	MathDocument::MathDocument()
		: m_current_pos(0)
	{

	}

	void MathDocument::type(const std::wstring& str)
	{
		m_text_buffer.insert(str, m_current_pos);
		m_current_pos += str.size();
	}

	void MathDocument::delBackward()
	{
		if (m_current_pos == 0)
		{
			return;
		}
		m_text_buffer.del(m_current_pos - 1, m_current_pos);
		--m_current_pos;
	}

	void MathDocument::delForward()
	{
		if (m_current_pos == m_text_buffer.getSize())
		{
			return;
		}
		m_text_buffer.del(m_current_pos, m_current_pos + 1);
	}

	void MathDocument::draw(VisualToolkit* visual_toolkit)
	{
		std::vector<FGlyphData> glyph_data;
		MeContainer container;
		MeParser parser(m_text_buffer, 0);
		while (true)
		{
			std::vector<std::unique_ptr<MeBase>> line;
			bool result = parser.parseLine(line);
			if (!result)
			{
				break;
			}
			container.addLine(std::move(line));
			container.calcLine(visual_toolkit);
		}
		container.draw(visual_toolkit);
	}
}
