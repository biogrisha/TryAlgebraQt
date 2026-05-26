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
			path.pop_back();
			ContPos& cont_pos = std::get<ContPos>(path.back());
			std::optional<size_t> child_pos = MeHelpers::absToChildPos(this, cont_pos.from);
			if (child_pos == 0)
			{
				if (dir == StepDir::right || dir == StepDir::down)
				{
					const auto& second_cont = getChildren()[1];
					cont_pos.from = second_cont->getChFrom();
					path.push_back(LeafPos(second_cont->getChFrom()));
				}
				else
				{
					path.pop_back();
					MePos& me_pos = std::get<MePos>(path.back());
					path.back() = LeafPos(me_pos.from);
				}
			}
			else if(child_pos == 1)
			{
				if (dir == StepDir::left || dir == StepDir::up)
				{
					const auto& first_cont = getChildren()[0];
					cont_pos.from = first_cont->getChFrom();

					auto& cont_children = first_cont->getChildren();
					if(cont_children.empty())
					{
						path.push_back(LeafPos(first_cont->getChFrom()));
					}
					else
					{
						path.push_back(LeafPos(cont_children.back()->getChTo()));
					}
				}
				else
				{
					path.pop_back();
					MePos& me_pos = std::get<MePos>(path.back());
					path.back() = LeafPos(me_pos.to);
				}
			}
			
		}
		else if (step_from == StepFrom::outside)
		{
			//path points at this
			path.back() = MePos(getChFrom(), getChTo());
			if (dir == StepDir::left)
			{
				auto& second_cont = getChildren()[1];
				auto& cont_children = second_cont->getChildren();
				path.push_back(ContPos(second_cont->getChFrom()));
				if (cont_children.empty())
				{
					path.push_back(LeafPos(second_cont->getChFrom()));
				}
				else
				{
					path.push_back(LeafPos(cont_children.back()->getChTo()));
				}
			}
			else if (dir == StepDir::right)
			{
				path.push_back(ContPos(getChildren()[0]->getChFrom()));
				path.push_back(LeafPos(getChildren()[0]->getChFrom()));
			}
		}
	}
}
