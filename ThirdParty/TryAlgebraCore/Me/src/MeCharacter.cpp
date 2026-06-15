#include <Me/include/MeCharacter.h>
#include <Me/include/MeGlobals.h>
#include <FreeTypeWrap.h>

namespace TryAlgebraCore
{
	MeCharacter::MeCharacter(wchar_t ch)
		:m_ch(ch)
	{
		m_glyph.GlyphId.Glyph = (ch == L' ') ? L'M' : ch;
	}

	void MeCharacter::calculate(VisualToolkit* visual_toolkit)
	{
		uint32_t font_size = std::max(uint32_t(g_font_def_height * m_scaling_factor), g_min_font_size);
		m_glyph.GlyphId.Height = font_size;
		auto render_data = visual_toolkit->ft->GetGlyphRenderData(m_glyph.GlyphId);
		if(render_data)
		{
			m_size.y = render_data->HeightInPixels;
			m_size.x = render_data->WidthInPixels;
			m_bearing_y = m_size.y / 2;
		}
	}

	void MeCharacter::draw(VisualToolkit* visual_toolkit)
	{
		if(m_ch != L' ')
		{
			m_glyph.Pos = m_pos;
			visual_toolkit->mdocState->AddGlyph(m_glyph);
		}
	}

	std::wstring MeCharacter::getName()
	{
		return std::wstring(1, m_ch);
	}

}
