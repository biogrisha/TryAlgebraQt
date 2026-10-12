#pragma once
#include <Me/include/MeBase.h>
#include <MathDocumentRenderingStructs.h>

namespace TryAlgebraCore
{
	enum class ScriptType
	{
		Subscript,
		Superscript
	};

	class MeScript : public MeBase
	{
		TYPED_CLASS1(MeBase)
	public:
		explicit MeScript(ScriptType type);
		void calculate(VisualToolkit* visual_toolkit) override;
		void step(StepDir dir, StepFrom step_from, MePath& path) override;
		ScriptType type() const;
	private:
		ScriptType m_type = ScriptType::Subscript;
	};
}