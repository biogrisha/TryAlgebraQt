#pragma once
#include <Me/include/MeBase.h>
#include <MathDocumentRenderingStructs.h>

namespace TryAlgebraCore2
{
	class MeCharacter : public MeBase
	{
		TYPED_CLASS1(MeBase)
	public:
		MeCharacter(const std::wstring& str);
		virtual void calculate(float size_scale, VisualToolkit* visual_toolkit) override;
		virtual void draw(VisualToolkit* visual_toolkit) override;
	public:
		static const uint32_t font_def_height = 16;
	private:
		std::wstring m_str;
		std::vector<FGlyphData> m_glyphs;
	};
}