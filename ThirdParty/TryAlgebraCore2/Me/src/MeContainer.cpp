#include <Me/include/MeContainer.h>

namespace TryAlgebraCore2
{
	void MeContainer::addLine(std::vector<std::unique_ptr<MeBase>>&& line)
	{
		m_lines.push_back(std::move(line));
	}
	bool MeContainer::calcLine(VisualToolkit* visual_toolkit)
	{
		float y_pos = 0;
		float max_y = 0;
		float max_bearing = 0;
		for (auto& me : m_lines.back())
		{
			me->calculate(1, visual_toolkit);
			me->setPosY(y_pos - me->getBearingY());
			max_y = std::max(me->getSizeY(), max_y);
			max_bearing = std::max(me->getBearingY(), max_bearing);
		}
		for (auto& me : m_lines.back())
		{
			me->setPosY(me->getPosY() + max_bearing);
		}
		next_line_y += max_y;
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
}
