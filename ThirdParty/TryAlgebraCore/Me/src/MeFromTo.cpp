#include <Me/include/MeFromTo.h>
#include <Me/include/MeGlobals.h>
#include <FreeTypeWrap.h>
#include <Helpers/include/MeHelpers.h>

namespace TryAlgebraCore
{
	void MeFromTo::setMeta(const std::wstring& meta)
	{
		m_glyph.GlyphId.Glyph = meta.back();
	}
	void MeFromTo::calculate(VisualToolkit* visual_toolkit)
	{
		assert(m_children.size() == 2);
		float ch_scaling_factor = m_scaling_factor * 0.3f;
		for (auto& ch : m_children)
		{
			ch->setScalingFactor(ch_scaling_factor);
			ch->calculate(visual_toolkit);
		}
		uint32_t font_size = std::max(uint32_t(g_font_def_height * m_scaling_factor * m_symbol_height_factor), g_min_font_size);
		m_glyph.GlyphId.Height = font_size;
		auto render_data = visual_toolkit->ft->GetGlyphRenderData(m_glyph.GlyphId);
		if (render_data)
		{
			m_symbol_width = render_data->WidthInPixels;
			float center = m_symbol_width / 2.f;
			MeHelpers::alignVertically(m_children, center);
			m_children[0]->setPosY(0);
			m_children[1]->setPosY(m_children[0]->getSize().y + render_data->HeightInPixels);
			setSizeY(m_children[1]->getPos().y + m_children[1]->getSize().y);
			setSizeX(center * 2.f);
		}
		
	}
	void MeFromTo::draw(VisualToolkit* visual_toolkit)
	{
		for (auto& ch : m_children)
		{
			ch->setPos(ch->getPos() + getPos());
			ch->draw(visual_toolkit);
		}
		visual_toolkit->mdoc_state->AddGlyph(m_glyph);
	}
}
