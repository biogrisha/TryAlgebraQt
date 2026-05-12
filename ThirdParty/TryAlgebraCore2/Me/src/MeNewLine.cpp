#include <Me/include/MeNewLine.h>
#include <Me/include/MeGlobals.h>
#include <MathDocumentState.h>
#include <FreeTypeWrap.h>

namespace TryAlgebraCore2
{
	void MeNewLine::calculate(float size_scale, VisualToolkit* visual_toolkit)
	{
		uint32_t font_size = std::max(uint32_t(font_def_height * size_scale), min_font_size);
		FGlyphId gl;
		gl.Glyph = L'M';
		gl.Height = font_size;
		auto render_data = visual_toolkit->ft->GetGlyphRenderData(gl);
		if (render_data)
		{
			m_size.y = render_data->HeightInPixels;
			m_size.x = 0;
			m_size *= size_scale;
			m_bearing_y = m_size.y / 2;
		}
	}
}
