#include <Me/include/MeContainer.h>
#include <FreeTypeWrap.h>
#include <Me/include/MeCharacter.h>
#include <Me/include/MeGlobals.h>
#include <Me/include/MeNewLine.h>
#include <Helpers/include/MeHelpers.h>
#include <algorithm>

namespace TryAlgebraCore
{
	void MeContainer::calcLine(VisualToolkit* visual_toolkit, size_t end)
	{
		float max_bearing = 0;
		float x = 0;
		for (int i = end_line_i; i < std::min(m_children.size(), end); ++i)
		{
			auto& ch = m_children[i];
			ch->setScalingFactor(m_scaling_factor);
			ch->calculate(visual_toolkit);
			max_bearing = std::max(max_bearing, ch->getBearingY());
			ch->setPosY(-ch->getBearingY());
			ch->setPosX(x);
			x += ch->getSize().x;
		}
		float max_y_offset = 0;
		for (int i = end_line_i; i < std::min(m_children.size(), end); ++i)
		{
			auto& ch = m_children[i];
			ch->setPosY(ch->getPos().y + next_line_y + max_bearing);
			max_y_offset = std::max(max_y_offset, ch->getPos().y + ch->getSize().y);
		}
		next_line_y = max_y_offset;
		m_size.x = std::max(x, m_size.x);
		m_size.y = next_line_y;
		end_line_i = std::min(m_children.size(), end);
	}
	void MeContainer::addEmptyLine()
	{
		m_size.y += g_caret_def_size.y * m_scaling_factor;
	}
	void MeContainer::draw(VisualToolkit* visual_toolkit)
	{
		for (auto& ch : m_children)
		{
			ch->setPos(ch->getPos() + getPos());
			ch->draw(visual_toolkit);
		}
		FRectInst rect;
		rect.Color = { 1, 1, 0, 0.3 };
		rect.Pos = getPos();
		rect.Size = getSize();
		visual_toolkit->mdoc_state->AddRect(rect);
	}
	void MeContainer::step(StepDir dir, StepFrom step_from, AbsPath& path)
	{
		MeHelpers::absToChildPos()
		if (dir == StepDir::right)
		{
			//not last -> next -> has child -> inside
			//                    no child  -> next
			//last -> parent.outside

		}
		else if (dir == StepDir::left)
		{
			
		}

	}
	void MeContainer::calculate(VisualToolkit* visual_toolkit)
	{
		bool ends_with_new_line = false;
		for (int i = 0; i < m_children.size(); ++i)
		{
			if (MyRTTI::Is<MeNewLine>(m_children[i].get()))
			{
				calcLine(visual_toolkit, i + 1);
				ends_with_new_line = (i == m_children.size() - 1);
			}
		}
		if (ends_with_new_line)
		{
			addEmptyLine();
		}
		else
		{
			calcLine(visual_toolkit);
		}
		setSizeX(std::max(getSize().x, m_scaling_factor * 10));
		setSizeY(std::max(getSize().y, m_scaling_factor * g_caret_def_size.y));
	}
}
