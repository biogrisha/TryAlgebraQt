#pragma once
#include <MyRTTI.h>
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include <vector>
#include <MathDocumentState.h>

namespace TryAlgebraCore2
{
	class MeBase
	{
		TYPED_CLASS
	public:
		virtual void draw(FMathDocumentState* render_data) {};
	protected:
		glm::vec2 pos;
		std::vector<std::unique_ptr<MeBase>> children;
	};
}