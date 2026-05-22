#pragma once

#include <Me/include/MeBase.h>
#include <MathDocumentRenderingStructs.h>

namespace TryAlgebraCore
{
	class MeFromTo : public MeBase
	{
		TYPED_CLASS1(MeBase)
	public:
		virtual void setMeta(const std::wstring& meta) override;
		virtual void calculate(VisualToolkit* visual_toolkit) override;
		virtual void draw(VisualToolkit* visual_toolkit) override;
		virtual void step(StepDir dir, StepFrom step_from, MePath& path) override;
	private:
		FGlyphData m_glyph;
		float m_symbol_height_factor = 2;
		float m_symbol_width = 0;
	};
}