#pragma once 
#include <Me/include/MeBase.h>

struct VisualToolkit;
namespace TryAlgebraCore
{
	class MeAtlas : public MeBase
	{
		TYPED_CLASS1(MeBase)

	public:
		MeAtlas(float max_width);
		virtual void calculate(VisualToolkit* visual_toolkit) override;
	private:
		float m_max_width;
	};
}