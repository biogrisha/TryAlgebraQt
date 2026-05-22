#include <Me/include/MeFromTo.h>
#include <Me/include/MeGlobals.h>
#include <FreeTypeWrap.h>
#include <Helpers/include/MeHelpers.h>

namespace TryAlgebraCore
{
	void MeFromTo::setMeta(const std::wstring& meta)
	{
		m_glyph.GlyphId.Glyph = meta.back();
		m_glyph.GlyphId.bCompact = true;
	}
	void MeFromTo::calculate(VisualToolkit* visual_toolkit)
	{
		assert(m_children.size() == 2);
		float ch_scaling_factor = m_scaling_factor / 1.5f;
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
			m_glyph.Pos.x = center - m_symbol_width/2;
			m_glyph.Pos.y = m_children[0]->getSize().y;
			setBearing(m_children[0]->getSize().y + render_data->HeightInPixels / 2.f);
		}
		
	}
	void MeFromTo::draw(VisualToolkit* visual_toolkit)
	{
		for (auto& ch : m_children)
		{
			ch->setPos(ch->getPos() + getPos());
			ch->draw(visual_toolkit);
		}
		m_glyph.Pos += getPos();
		visual_toolkit->mdoc_state->AddGlyph(m_glyph);
	}
	void MeFromTo::step(StepDir dir, StepFrom step_from, MePath& path)
	{
		//handling container selection
		if (step_from == StepFrom::inside)
		{
			ContPos& cont_pos = std::get<ContPos>(path.back());
			std::optional<size_t> child_pos = MeHelpers::absToChildPos(this, cont_pos.from);
			if (child_pos == 0)
			{
				if (dir == StepDir::right || dir == StepDir::down)
				{
					const auto& child = getChildren()[1];
					cont_pos.from = child->getChFrom();
					path.emplace_back(0, 0);
				}
				else
				{
					path.pop_back();
				}
			}
			else
			{
				if (dir == StepDir::left || dir == StepDir::up)
				{
					const auto& child = getChildren()[0];
					path.back().from = child->getChFrom();
					if(child->getChildren().empty())
					{
						path.emplace_back(0, 0);
					}
					else
					{

					}
				}
				else
				{
					path.pop_back();
				}
			}
			
		}

	}
}
