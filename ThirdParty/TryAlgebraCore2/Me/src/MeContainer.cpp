#include <Me/include/MeContainer.h>
#include <FreeTypeWrap.h>
#include <Me/include/MeCharacter.h>

namespace TryAlgebraCore2
{
	void MeContainer::addLine(std::vector<std::unique_ptr<MeBase>>&& line)
	{
		m_lines.push_back(std::move(line));
	}
	bool MeContainer::calcLine(VisualToolkit* visual_toolkit)
	{
		//handle empty line
		if (m_lines.back().empty())
		{
			next_line_y += visual_toolkit->ft->GetHeightFromFontSize(MeCharacter::font_def_height * 1);
			return true;
		}

		float y_pos = 0;
		float max_y = 0;
		float max_bearing = 0;
		for (auto& me : m_lines.back())
		{
			me->calculate(1, visual_toolkit);
			me->setPosY(y_pos - me->getBearingY());
			max_y = std::max(me->getSizeY() - me->getBearingY(), max_y);
			max_bearing = std::max(me->getBearingY(), max_bearing);
		}
		for (auto& me : m_lines.back())
		{
			me->setPosY(me->getPosY() + max_bearing + next_line_y);
		}
		next_line_y += max_y + max_bearing;
		return true;
	}
	void MeContainer::draw(VisualToolkit* visual_toolkit)
	{
		for (auto& line : m_lines)
		{
			for (auto& me : line)
			{
				me->draw(visual_toolkit);
			}
		}
	}
	void MeContainer::step(StepDir dir, StepFrom step_from, std::vector<int>& path)
	{

		//last, next cont (neighbour(start,end),grandparent(next,prev))
		//up - line above,next cont(neighbour(start,end),grandparent(next,prev))
		//down - line below, next cont(neighbour(start,end),grandparent(next,prev))
		//next - next me, in cont child
		//prev -> prev me, in cont prev child

		if (dir == StepDir::right)
		{
			if (path.back() == m_children.size())
			{
				if (m_parent)
				{
					m_parent->step(dir, StepFrom::inside, path);
				}
			}
			else if (!m_children[path.back()]->getChildren().empty())
			{
				m_children[path.back()]->step(dir, StepFrom::outside, path);
			}
			else
			{
				++path.back();
			}
		}
		else if (dir == StepDir::left)
		{
			if (path.back() == 0)
			{
				if (m_parent)
				{
					m_parent->step(dir, StepFrom::inside, path);
				}
			}
			else if (!m_children[path.back() - 1]->getChildren().empty())
			{
				m_children[path.back() - 1]->step(dir, StepFrom::outside, path);
			}
			else
			{
				--path.back();
			}
		}

	}
}
