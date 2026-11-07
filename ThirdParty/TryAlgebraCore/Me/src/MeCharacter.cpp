#include <Me/include/MeCharacter.h>
#include <Me/include/MeGlobals.h>
#include <FreeTypeWrap.h>

namespace TryAlgebraCore
{
	MeCharacter::MeCharacter(wchar_t ch)
		:m_ch(ch)
	{
		m_glyph.GlyphId.Glyph = (ch == L' ') ? L'I' : ch;
	}

	void MeCharacter::calculate(VisualToolkit* visual_toolkit)
	{
		uint32_t font_size = std::max(uint32_t(g_font_def_height * m_scaling_factor), g_min_font_size);
		m_glyph.GlyphId.Height = font_size;
		m_glyph.GlyphId.bCompact = true;
		auto render_data = visual_toolkit->ft->GetGlyphRenderData(m_glyph.GlyphId);
		if (render_data)
		{
			auto& metrics = render_data->metrics;
			auto width = metrics.width / 64;
			auto height = metrics.height / 64;

			m_size.y = height;
			m_size.x = width;
			m_bearing_y = metrics.horiBearingY / 64;
		}
	}

	void MeCharacter::draw(VisualToolkit* visual_toolkit)
	{
		if (m_ch != L' ')
		{
			m_glyph.Pos = glm::ceil(m_pos);
			visual_toolkit->mdocState->at(1).addGlyph(m_glyph);
		}
	}

	std::wstring MeCharacter::getName()
	{
		return std::wstring(1, m_ch);
	}

}
