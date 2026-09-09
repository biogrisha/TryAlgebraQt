#pragma once
#include <Me/include/MeBase.h>
#include <MathDocumentRenderingStructs.h>

namespace TryAlgebraCore
{
	class MeBracket : public MeBase
	{
		TYPED_CLASS1(MeBase)
	public:
		MeBracket(wchar_t ch);
		virtual void calculate(VisualToolkit* visual_toolkit) override;
		virtual void draw(VisualToolkit* visual_toolkit) override;
		void setHeight(float height);
		wchar_t ch() const { return m_ch; }
	private:
		wchar_t m_ch;
		std::vector<FOutlineCurvePoints> m_points;
	};
}