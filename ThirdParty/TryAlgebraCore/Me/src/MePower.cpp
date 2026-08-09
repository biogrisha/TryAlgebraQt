#include <Me/include/MePower.h>
#include <FreeTypeWrap.h>
#include <Me/include/MeGlobals.h>
#include <Helpers/include/MeHelpers.h>

namespace TryAlgebraCore {

	void MePower::calculate(VisualToolkit* vt)
	{
		float chScalingFactor = m_scaling_factor / 1.5f;
		m_children[0]->setScalingFactor(chScalingFactor);
		m_children[0]->calculate(vt);

		FGlyphId g;
		g.Glyph = L'M';
		g.Height = g_font_def_height * m_scaling_factor;
		auto height = vt->ft->GetGlyphSize(g).y;
		setSize(m_children[0]->getSize());
		setBearing(height / 2);
	}

	void MePower::step(StepDir dir, StepFrom step_from, MePath& path)
	{
		MeHelpers::defaultStep(this, dir, step_from, path);
	}

	std::wstring MePower::getName()
	{
		return MeNames::power;
	}

}
