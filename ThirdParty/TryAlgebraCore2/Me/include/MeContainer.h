#pragma once
#include <Me/include/MeBase.h>
#include <vector>

namespace TryAlgebraCore2
{
	class MeContainer : public MeBase
	{
		TYPED_CLASS1(MeBase)

	public:
		void addLine(std::vector<std::unique_ptr<MeBase>>&& line);
		bool calcLine(VisualToolkit* visual_toolkit);
		virtual void draw(VisualToolkit* visual_toolkit) override;
		virtual void step(StepDir dir, StepFrom step_from, std::vector<int>& path) override;
	private:
		std::vector<std::vector<std::unique_ptr<MeBase>>> m_lines;
		float next_line_y = 0;
	};
}