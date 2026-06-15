#pragma once
#include <Me/include/MeBase.h>
#include <MathDocumentRenderingStructs.h>

namespace TryAlgebraCore
{
	class MeVariable : public MeBase
	{
		TYPED_CLASS1(MeBase)
	public:
		virtual void calculate(VisualToolkit* visual_toolkit) override;
		virtual void draw(VisualToolkit* visual_toolkit) override;
		virtual void step(StepDir dir, StepFrom step_from, MePath& path) override;
		virtual std::wstring getName() override;
	private:
		FGlyphData m_glyph;
	};
}