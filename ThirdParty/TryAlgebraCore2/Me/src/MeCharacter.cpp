#include <Me/include/MeCharacter.h>
#include <Me/include/MeGlobals.h>
#include <FreeTypeWrap.h>

namespace TryAlgebraCore2
{
	MeCharacter::MeCharacter(const std::wstring& str)
		:m_str(str)
	{
		m_glyphs.resize(str.size());
		for (int i = 0; i < str.size(); ++i)
		{
			m_glyphs[i].GlyphId.Glyph = str[i];
		}
	}

	void MeCharacter::calculate(float size_scale, VisualToolkit* visual_toolkit)
	{
		assert(!m_glyphs.empty());
		uint32_t font_size = std::max(uint32_t(font_def_height * size_scale), min_font_size);

		FGlyphRenderData* render_data = nullptr;
		for (int i = 0; i < m_glyphs.size(); ++i)
		{
			auto& gl = m_glyphs[i].GlyphId;
			gl.Height = font_size;
			if (gl.Glyph == L' ')
			{
				gl.Glyph = L'M';
				render_data = visual_toolkit->ft->GetGlyphRenderData(gl);
				gl.Glyph = L' ';
			}
			else
			{
				render_data = visual_toolkit->ft->GetGlyphRenderData(gl);
			}
			m_glyphs[i].Pos = { m_size.x, 0 };
			m_size.x += render_data->WidthInPixels;
		}
		m_size.y = render_data->HeightInPixels;
		m_bearing_y = m_size.y / 2;
	}

	void MeCharacter::draw(VisualToolkit* visual_toolkit)
	{
		for (int i = m_glyphs.size() - 1; i >= 0; --i)
		{
			if (m_glyphs[i].GlyphId.Glyph == L' ')
			{
				std::swap(m_glyphs[i], m_glyphs.back());
				m_glyphs.pop_back();
				continue;
			}
			m_glyphs[i].Pos += m_pos;
		}
		visual_toolkit->mdoc_state->appendGlyphs(m_glyphs);
	}

}
