#pragma once
#include <Me/include/MeBase.h>

namespace TryAlgebraCore
{
	class MeOverset : public MeBase
	{
		TYPED_CLASS1(MeBase)
	public:
		void calculate(VisualToolkit* visual_toolkit) override;
		void step(StepDir dir, StepFrom step_from, MePath& path) override;
	};
}