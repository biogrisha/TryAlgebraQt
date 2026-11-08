#include <Me/include/MeVariable.h>
#include <Me/include/MeGlobals.h>
#include <Helpers/include/MeHelpers.h>

namespace TryAlgebraCore
{
	void MeVariable::setMeta(const std::wstring& meta)
	{
		if (meta == MeNames::varUni)
		{
			m_type = Type::Uni;
		}
		else if (meta == MeNames::varZeroMulti)
		{
			m_type = Type::ZeroMulti;
		}
		else if (meta == MeNames::varOneMulti)
		{
			m_type = Type::OneMulti;
		}
	}
	void MeVariable::calculate(VisualToolkit* visual_toolkit)
	{
		m_children[0]->setScalingFactor(m_scaling_factor);
		m_children[0]->calculate(visual_toolkit);
		setSize(m_children[0]->getSize());
		if (m_children[0]->getChildren().empty())
		{
			setBearing(getSize().y);
		}
		else
		{
			auto childPos = m_children[0]->getChildren()[0]->getPos();
			auto childBearing = m_children[0]->getChildren()[0]->getBearingY();
			setBearing(childPos.y + childBearing);
		}
	}

	void MeVariable::draw(VisualToolkit* visual_toolkit)
	{
		FRectInst rect;
		switch (m_type)
		{
		case Type::Uni:
			rect.Color = { 1,1,0.5,0.8 };
			break;
		case Type::ZeroMulti:
			rect.Color = { 1,0.5,1,0.8 };
			break;
		case Type::OneMulti:
			rect.Color = { 0.5,1,1,0.8 };
			break;
		default:
			break;
		}

		rect.Pos = getPos();
		rect.Size = getSize();
		visual_toolkit->mdocState->at(1).addRectangle(rect);
		MeBase::draw(visual_toolkit);
	}

	void MeVariable::step(StepDir dir, StepFrom step_from, MePath& path)
	{
		MeHelpers::defaultStep(this, dir, step_from, path);
	}

	std::wstring MeVariable::getName()
	{
		return MeNames::variable;
	}

}
