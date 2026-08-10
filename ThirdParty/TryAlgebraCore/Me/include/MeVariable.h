#pragma once
#include <Me/include/MeBase.h>
#include <MathDocumentRenderingStructs.h>

namespace TryAlgebraCore
{
	class MeVariable : public MeBase
	{
		enum class Type
		{
			Uni,
			ZeroMulti,
			OneMulti,
		};

		TYPED_CLASS1(MeBase)
	public:
		void setMeta(const std::wstring& meta) override;
		void calculate(VisualToolkit* visual_toolkit) override;
		void draw(VisualToolkit* visual_toolkit) override;
		void step(StepDir dir, StepFrom step_from, MePath& path) override;
		std::wstring getName() override;
	private:
		FGlyphData m_glyph;
		Type m_type;
	};
}