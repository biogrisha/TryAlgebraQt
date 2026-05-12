#pragma once
#include <Me/include/MeBase.h>
#include <MathDocumentRenderingStructs.h>

namespace TryAlgebraCore2
{
	class MeCharacter : public MeBase
	{
		TYPED_CLASS1(MeBase)
	public:
		MeCharacter(wchar_t ch);
		virtual void calculate(float size_scale, VisualToolkit* visual_toolkit) override;
		virtual void draw(VisualToolkit* visual_toolkit) override;
	private:
		wchar_t m_ch;
		FGlyphData m_glyph;
	};
}