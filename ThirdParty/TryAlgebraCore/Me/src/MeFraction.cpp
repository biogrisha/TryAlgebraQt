#include <Me/include/MeFraction.h>
#include <Helpers/include/MeHelpers.h>
#include <Me/include/MeGlobals.h>

namespace TryAlgebraCore {

	void MeFraction::calculate(VisualToolkit* vt)
	{
		assert(m_children.size() == 2);
		for (auto& ch : m_children)
		{
			ch->setScalingFactor(m_scaling_factor);
			ch->calculate(vt);
		}
		float center = 0;
		float lineWidthScaled = m_scaling_factor * lineWidth;
		MeHelpers::alignVertically(m_children, center);
		m_children[0]->setPosY(0);
		m_children[1]->setPosY(m_children[0]->getSize().y + lineWidthScaled);
		setSizeY(m_children[1]->getPos().y + m_children[1]->getSize().y);
		setSizeX(center * 2.f);
		setBearing(m_children[0]->getSize().y + lineWidthScaled / 2.f);
	}
	void MeFraction::draw(VisualToolkit* vt)
	{
		LineChain line;
		line.color = { 1,1,1,1 };
		line.width = m_scaling_factor * lineWidth;
		auto chSize = m_children.front()->getSize();
		line.points.push_back(getPos() + glm::vec2{ 0,chSize.y });
		line.points.push_back(getPos() + glm::vec2{ getSize().x,chSize.y });
		vt->mdocState->at(1).addLine(std::move(line));
		MeBase::draw(vt);
	}
	void MeFraction::step(StepDir dir, StepFrom step_from, MePath& path)
	{
		MeHelpers::defaultStepHorizontal(this, dir, step_from, path);
	}
	std::wstring MeFraction::getName()
	{
		return MeNames::fraction;
	}
}
