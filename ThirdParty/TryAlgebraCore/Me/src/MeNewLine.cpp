#include <Me/include/MeNewLine.h>
#include <Me/include/MeGlobals.h>
#include <MathDocumentState.h>
#include <FreeTypeWrap.h>

namespace TryAlgebraCore
{
	void MeNewLine::calculate(VisualToolkit* visual_toolkit)
	{
		uint32_t font_size = std::max(uint32_t(g_font_def_height * m_scaling_factor), g_min_font_size);
		FGlyphId gl;
		gl.Glyph = L'M';
		gl.Height = font_size;
		auto render_data = visual_toolkit->ft->GetGlyphRenderData(gl);
		if (render_data)
		{
			auto& metrics = render_data->metrics;
			auto height = metrics.height / 64;
			m_size.y = height;
			m_size.x = 0;
			m_size *= m_scaling_factor;
			m_bearing_y = m_size.y;
		}
	}
}
