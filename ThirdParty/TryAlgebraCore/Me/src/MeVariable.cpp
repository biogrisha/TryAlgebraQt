#include <Me/include/MeVariable.h>
#include <Me/include/MeGlobals.h>
#include <Helpers/include/MeHelpers.h>

namespace TryAlgebraCore
{

void MeVariable::calculate(VisualToolkit* visual_toolkit)
{
	float padding = 2;
	m_children[0]->setScalingFactor(m_scaling_factor);
	m_children[0]->calculate(visual_toolkit);
	m_children[0]->setPos({ padding, padding });
	setSize(m_children[0]->getSize() + glm::vec2{ padding * 2, padding  * 2});
	setBearing(getSize().y / 2.);
}

void MeVariable::draw(VisualToolkit* visual_toolkit)
{
	FRectInst rect;
	rect.Color = { 1,1,0,0.5 };

	rect.Pos = getPos();
	rect.Size = getSize();
	visual_toolkit->mdocState->AddCosmeticRect(rect);
	MeBase::draw(visual_toolkit);
}

void MeVariable::step(StepDir dir, StepFrom step_from, MePath& path)
{
	//handling container selection
	if (step_from == StepFrom::inside)
	{
		path.pop_back();
		if (dir == StepDir::left || dir == StepDir::up)
		{
			path.pop_back();
			MePos& me_pos = std::get<MePos>(path.back());
			path.back() = LeafPos(me_pos.from);
			
		}
		else if (dir == StepDir::right || dir == StepDir::down)
		{
			path.pop_back();
			MePos& me_pos = std::get<MePos>(path.back());
			path.back() = LeafPos(me_pos.to);
			
		}
	}
	else if (step_from == StepFrom::outside)
	{
		//path points at this
		path.back() = MePos(getChFrom(), getChTo());
		if (dir == StepDir::left)
		{
			auto& second_cont = getChildren()[0];
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

std::wstring MeVariable::getName()
{
	return MeNames::variable;
}

}
