#pragma once
#include <MyRTTI.h>
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include <vector>
#include <MathDocumentState.h>
#include <MathEditor/include/Structs.h>


namespace TryAlgebraCore
{
	
	enum class StepDir
	{
		left,
		right,
		up,
		down
	};

	enum class StepFrom
	{
		outside,
		inside,
		none
	};
	class MeBase
	{
		TYPED_CLASS
	public:
		virtual void setMeta(const std::wstring& meta) {}
		virtual void draw(VisualToolkit* visual_toolkit) {}
		virtual void calculate(VisualToolkit* visual_toolkit) {}
		virtual void step(StepDir dir, StepFrom step_from, std::vector<AbsPathEl>& path) {}
		void addChild(std::unique_ptr<MeBase>&& child);
		const glm::vec2& getSize() { return m_size; }
		const glm::vec2& getPos() { return m_pos; }
		void setSize(const glm::vec2& size) { m_size = size; }
		void setPos(const glm::vec2& pos) { m_pos = pos; }
		void setPosX(float x) { m_pos.x = x; }
		void setPosY(float y) { m_pos.y = y; }

		void setSizeY(float y) { m_size.y = y; }
		void setSizeX(float x) { m_size.x = x; }

		float getBearingY() { return m_bearing_y; }
		std::vector<std::unique_ptr<MeBase>>& getChildren() { return m_children; }
		void setChFrom(int i) { m_ch_from = i; }
		int getChFrom() { return m_ch_from; }
		void setChTo(int i) { m_ch_to = i; }
		int getChTo() { return m_ch_to; }
		float getScalingFactor() { return m_scaling_factor; }
		void setScalingFactor(float val) { m_scaling_factor = val; }
		void setParent(MeBase* parent) { m_parent = parent; }
		MeBase* getParent() { return m_parent; }
	protected:
		glm::vec2 m_pos = { 0, 0 };
		glm::vec2 m_size = { 0, 0 };
		float m_bearing_y = 0;
		std::vector<std::unique_ptr<MeBase>> m_children;
		MeBase* m_parent = nullptr;
		int m_ch_from = -1;
		int m_ch_to = -1;
		float m_scaling_factor = 1.f;

	};
}