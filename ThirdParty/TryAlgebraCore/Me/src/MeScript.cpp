#include <Me/include/MeScript.h>
#include <FreeTypeWrap.h>
#include <Me/include/MeGlobals.h>
#include <Helpers/include/MeHelpers.h>

namespace TryAlgebraCore {
	MeScript::MeScript(ScriptType type)
		: m_type(type)
	{
	}

	void MeScript::calculate(VisualToolkit* vt)
	{
		float chScalingFactor = m_scaling_factor / 1.5f;
		m_children[0]->setScalingFactor(chScalingFactor);
		m_children[0]->calculate(vt);
		setSize(m_children[0]->getSize());
		setBearing(getSize().y / 2);
	}

	void MeScript::step(StepDir dir, StepFrom step_from, MePath& path)
	{
		MeHelpers::defaultStep(this, dir, step_from, path);
	}

	ScriptType MeScript::type() const
	{
		return m_type;
	}
}
