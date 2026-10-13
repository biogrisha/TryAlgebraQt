#pragma once
#include <Me/include/MeBase.h>
#include <MathDocumentRenderingStructs.h>

namespace TryAlgebraCore
{
	class MeMatrix : public MeBase
	{
		TYPED_CLASS1(MeBase)
	public:
		void setMeta(const std::wstring& meta) override;
		void calculate(VisualToolkit* visual_toolkit) override;
		void step(StepDir dir, StepFrom step_from, MePath& path) override;
	private:
		int m_rows = 0;
		int m_columns = 0;
	};
}
