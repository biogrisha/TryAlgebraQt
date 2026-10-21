#include <Me/include/MeOverset.h>
#include <Helpers/include/MeHelpers.h>
namespace TryAlgebraCore {
	void MeOverset::calculate(VisualToolkit* vt)
	{
		assert(m_children.size() == 2);
		m_children[0]->setScalingFactor(m_scaling_factor / 1.5f);
		m_children[1]->setScalingFactor(m_scaling_factor);
		for (auto& ch : m_children)
		{
			ch->calculate(vt);
		}
		float center = m_children[1]->getSize().x / 2.f;
		MeHelpers::alignVertically(m_children, center);
		m_children[0]->setPosY(0);
		m_children[1]->setPosY(m_children[0]->getSize().y - 5 * m_scaling_factor);
		setSizeY(m_children[1]->getPos().y + m_children[1]->getSize().y);
		setSizeX(center * 2.f);
		setBearing(m_children[1]->getPos().y + m_children[1]->getSize().y / 2.f);
	}
	void MeOverset::step(StepDir dir, StepFrom step_from, MePath& path)
	{
		MeHelpers::defaultStep(this, dir, step_from, path);
	}
}

