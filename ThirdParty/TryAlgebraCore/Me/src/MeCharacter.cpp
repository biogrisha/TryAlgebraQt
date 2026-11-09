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
			const float width = metrics.width / 64.f;
			const float outline_height = metrics.height / 64.f;
			const float min_height = visual_toolkit->ft->GetHeightFromFontSize(font_size) * 0.7;
			const float extra_height = std::max(min_height - outline_height, 0.f);

			m_size.y = outline_height + extra_height;
			m_size.x = width;
			m_glyph_y_offset = extra_height / 2.f;
			m_bearing_y = metrics.horiBearingY / 64.f + m_glyph_y_offset;
		}
	}

	void MeCharacter::draw(VisualToolkit* visual_toolkit)
	{
		if (m_ch != L' ')
		{
			m_glyph.Pos = glm::ceil(m_pos + glm::vec2{ 0.f, m_glyph_y_offset });
			visual_toolkit->mdocState->at(1).addGlyph(m_glyph);
		}
	}

	std::wstring MeCharacter::getName()
	{
		return std::wstring(1, m_ch);
	}

}
