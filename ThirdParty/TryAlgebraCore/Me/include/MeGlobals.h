#pragma once
#include <string>
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>

namespace TryAlgebraCore
{
	inline uint32_t g_min_font_size = 5;
	inline uint32_t g_font_def_height = 20;
	inline glm::vec2 g_caret_def_size = { 2, 20 };
	inline glm::vec2 g_invalid_caret_pos = { -100, -100 };
	namespace MeNames
	{
		inline std::wstring from_to = L"ft";
		inline std::wstring integral = L"∫";
	}
}