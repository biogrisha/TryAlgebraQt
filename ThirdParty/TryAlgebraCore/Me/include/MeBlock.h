#pragma once
#include <Me/include/MeBase.h>

namespace TryAlgebraCore
{
	class MeBlock : public MeBase
	{
		TYPED_CLASS1(MeBase)
	public:
		void calculate(VisualToolkit* visual_toolkit) override;
		void step(StepDir dir, StepFrom step_from, MePath& path) override;
		std::wstring getName() override;
	};
}
