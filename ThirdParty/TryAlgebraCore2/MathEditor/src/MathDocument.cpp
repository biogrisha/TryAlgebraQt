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
	}

	void MathDocument::draw(VisualToolkit* visual_toolkit)
	{
		std::vector<FGlyphData> glyph_data;
		MeContainer container;
		MeParser parser(m_text_buffer, 0);
		while (true)
		{
			auto new_line = parser.parseLine();
			if (new_line.empty())
			{
				break;
			}
			container.addLine(std::move(new_line));
			container.calcLine(visual_toolkit);
		}
		container.draw(visual_toolkit);
	}
}
