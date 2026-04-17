#pragma once
#include <MyRTTI.h>
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>

namespace TryAlgebraCore2
{
	class MeBase
	{
		TYPED_CLASS
	protected:
		//dynamic properties
		glm::vec2 local_pos;
		float size = 1;
		//static properties
	};
}