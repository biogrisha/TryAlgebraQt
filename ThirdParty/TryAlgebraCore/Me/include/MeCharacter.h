#pragma once
#include <Me/include/MeBase.h>
#include <MathDocumentRenderingStructs.h>

namespace TryAlgebraCore
{
	class MeCharacter : public MeBase
	{
		TYPED_CLASS1(MeBase)
	public:
		MeCharacter(wchar_t ch);
		virtual void calculate(VisualToolkit* visual_toolkit) override;
		virtual void draw(VisualToolkit* visual_toolkit) override;
		virtual std::wstring getName() override;
	private:
		wchar_t m_ch;
		FGlyphData m_glyph;
		float m_glyph_y_offset = 0.f;
	};
}
