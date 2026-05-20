#include <Me/include/MeContainer.h>
#include <FreeTypeWrap.h>
#include <Me/include/MeCharacter.h>
#include <Me/include/MeGlobals.h>
#include <algorithm>

namespace TryAlgebraCore
{
	void MeContainer::calcLine(VisualToolkit* visual_toolkit)
	{
		float max_bearing = 0;
		float x = 0;
		for (int i = end_line_i; i < m_children.size(); ++i)
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
		for (int i = end_line_i; i < m_children.size(); ++i)
		{
			auto& ch = m_children[i];
			ch->setPosY(ch->getPos().y + next_line_y + max_bearing);
			max_y_offset = std::max(max_y_offset, ch->getPos().y + ch->getSize().y);
		}
		next_line_y = max_y_offset;
		m_size.x = std::max(x, m_size.x);
		m_size.y = next_line_y;
		end_line_i = m_children.size();
	}
	void MeContainer::addEmptyLine()
	{
		m_size.y += g_caret_height * m_scaling_factor;
	}
	void MeContainer::draw(VisualToolkit* visual_toolkit)
	{
		for (auto& ch : m_children)
		{
			ch->setPos(ch->getPos() + getPos());
			ch->draw(visual_toolkit);
		}
	}
	void MeContainer::step(StepDir dir, StepFrom step_from, std::vector<AbsPathEl>& path)
	{

		//last, next cont (neighbour(start,end),grandparent(next,prev))
		//up - line above,next cont(neighbour(start,end),grandparent(next,prev))
		//down - line below, next cont(neighbour(start,end),grandparent(next,prev))
		//next - next me, in cont child
		//prev -> prev me, in cont prev child

		if (dir == StepDir::right)
		{
			/*auto it = std::lower_bound(
				m_children.begin(),
				m_children.end(),
				10,
				[](const std::unique_ptr<MeBase>& ch, int value) {
					return ch->getChFrom() < value;
				}
			);

			if (it != m_children.end() && it->id == targetId) {
				std::cout << "Found: " << it->name << '\n';
			}
			else {
				std::cout << "Not found\n";
			}*/
		}
		else if (dir == StepDir::left)
		{
			
		}

	}
}
