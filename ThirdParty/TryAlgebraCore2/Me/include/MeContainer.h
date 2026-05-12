#pragma once
#include <Me/include/MeBase.h>
#include <vector>

namespace TryAlgebraCore2
{
	class MeContainer : public MeBase
	{
		TYPED_CLASS1(MeBase)

	public:
		void calcLine(VisualToolkit* visual_toolkit, float size_scale);
		virtual void draw(VisualToolkit* visual_toolkit) override;
		virtual void step(StepDir dir, StepFrom step_from, std::vector<int>& path) override;
	private:
		float next_line_y = 0;
		int end_line_i = 0;
	};
}