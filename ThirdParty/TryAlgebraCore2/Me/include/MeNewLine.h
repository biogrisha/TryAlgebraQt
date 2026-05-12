#pragma once 
#include <Me/include/MeBase.h>

struct VisualToolkit;
namespace TryAlgebraCore2
{
	class MeNewLine : public MeBase
	{
		TYPED_CLASS1(MeBase)

	public:
		virtual void calculate(float size_scale, VisualToolkit* visual_toolkit) override;
	};
}