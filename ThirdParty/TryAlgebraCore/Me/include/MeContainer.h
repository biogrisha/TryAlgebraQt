#pragma once
#include <Me/include/MeBase.h>
#include <vector>

namespace TryAlgebraCore
{
	class MeContainer : public MeBase
	{
		TYPED_CLASS1(MeBase)

	public:
		virtual void draw(VisualToolkit* vt);
		virtual void step(StepDir dir, StepFrom step_from, MePath& path) override;
		virtual void calculate(VisualToolkit* visual_toolkit) override;
		virtual std::wstring getName() override;

		void calcLine(VisualToolkit* visual_toolkit, size_t end = UINT64_MAX);
		void addEmptyLine();
	private:
		float next_line_y = 0;
		int end_line_i = 0;
	};
}