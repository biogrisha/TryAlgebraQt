#include <Me/include/MeAtlas.h>

namespace TryAlgebraCore
{
	MeAtlas::MeAtlas(float max_width)
		: m_max_width(max_width)
	{

	}
	void MeAtlas::calculate(VisualToolkit* visual_toolkit)
	{
		float x = 0;
		float y = 0;
		float max_y = 0;
		for (const auto& ch : m_children)
		{
			float x_next = ch->getSize().x + x;
			if (x_next > m_max_width)
			{
				//this me does not fit by width
				// move to the next line
				y = max_y;
				x = 0;
			}
			ch->setScalingFactor(1);
			ch->calculate(visual_toolkit);
			ch->setPos({ x, y });
			max_y = std::max(y + ch->getSize().y, max_y);
			x += ch->getSize().x;
		}
		calculatePos();
		setSize({ m_max_width, max_y });
	}
}