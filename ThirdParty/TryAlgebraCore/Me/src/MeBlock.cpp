#include <Me/include/MeBlock.h>
#include <Me/include/MeContainer.h>
#include <Me/include/MeGlobals.h>
#include <Helpers/include/MeHelpers.h>

namespace TryAlgebraCore
{
	void MeBlock::calculate(VisualToolkit* visual_toolkit)
	{
		assert(m_children.size() == 1);
		auto* cont = MyRTTI::Cast<MeContainer>(m_children[0].get());
		assert(cont);
		cont->setDrawBorder(true);
		cont->setScalingFactor(m_scaling_factor);
		cont->setPos({ 0, 0 });
		cont->calculate(visual_toolkit);
		setSize(cont->getSize());
		const auto reference = MeHelpers::getReferenceLetterMetrics(visual_toolkit, m_scaling_factor);
		setBearing(getSize().y / 2.f + reference.centerAboveBaseline());
	}

	void MeBlock::step(StepDir dir, StepFrom step_from, MePath& path)
	{
		MeHelpers::defaultStep(this, dir, step_from, path);
	}

	std::wstring MeBlock::getName()
	{
		return MeNames::block;
	}
}
