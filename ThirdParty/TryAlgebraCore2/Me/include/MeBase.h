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
		virtual void draw(VisualToolkit* visual_toolkit) {}
		virtual void calculate(float size_scale, VisualToolkit* visual_toolkit) {}
		glm::vec2& getSize() { return m_size; }
		void setPosX(float x) { m_pos.x = x; }
		void setPosY(float y) { m_pos.y = y; }
		float getPosY() { return m_pos.y; }
		float getPosX() { return m_pos.x; }
		float getSizeY() { return m_size.y; }
		float getSizeX() { return m_size.x; }
		float getBearingY() { return m_bearing_y; }
	protected:
		glm::vec2 m_pos = { 0, 0 };
		glm::vec2 m_size = { 0, 0 };
		float m_bearing_y = 0;
		std::vector<std::unique_ptr<MeBase>> m_children;

	};
}