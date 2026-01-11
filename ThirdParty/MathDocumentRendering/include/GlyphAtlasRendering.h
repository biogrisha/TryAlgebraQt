#pragma once
#include "VertexInputLayout.h"
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include <Pipeline.h>
#include <DescriptorManager.h>
#include <Rendering.h>
#include "FreeTypeWrap.h"

struct FSimpleVertex
{
	glm::vec2 Pos;
};

struct FGlyphInstance
{
	glm::vec2 Offset;
	glm::vec2 Size;
	uint32_t StartIndex;
	uint32_t CurvesCount;
};

class FGlyphAtlasInputLayout : public FVertexInputLayout {
	virtual std::vector<vk::VertexInputBindingDescription> getBindingDescription() override {
		return {
			{ 0, sizeof(FSimpleVertex), vk::VertexInputRate::eVertex },
			{ 1, sizeof(FGlyphInstance), vk::VertexInputRate::eInstance },
		};
	}
	virtual std::vector<vk::VertexInputAttributeDescription> getAttributeDescriptions() override {
		return {
			vk::VertexInputAttributeDescription(0, 0, vk::Format::eR32G32Sfloat, offsetof(FSimpleVertex, Pos)),

			vk::VertexInputAttributeDescription(1, 1, vk::Format::eR32G32Sfloat, offsetof(FGlyphInstance, Offset)),
			vk::VertexInputAttributeDescription(2, 1, vk::Format::eR32G32Sfloat, offsetof(FGlyphInstance, Size)),
			vk::VertexInputAttributeDescription(3, 1, vk::Format::eR32Uint, offsetof(FGlyphInstance, StartIndex)),
			vk::VertexInputAttributeDescription(4, 1, vk::Format::eR32Uint, offsetof(FGlyphInstance, CurvesCount)),
		};
	}
};


class FGlyphAtlasRendering
{
public:
	void Init(FRendering* InRendering);
	void InitPLine();
	void SetExtent(const VkExtent2D& InExetent);
	void SetInstances(const std::vector<FGlyphInstance>& InInstances);
	void SetOutlineCurves(const std::vector<FOutlineCurvePoints>& InOutlineCurves);
	void Render();
	FImageBuffer* GetAtlas();
private:
	std::unique_ptr<FBuffer> VertexBuffer;
	std::unique_ptr<FBuffer> InstanceBuffer;
	std::unique_ptr<FBuffer> IndexBuffer;
	std::unique_ptr<FBuffer> OutlineBuffer;
	std::unique_ptr<FBuffer> UniformBuffer;
	std::unique_ptr<FImageBuffer> Atlas;
	std::vector<FOutlineCurvePoints> OutlineCurves;
	std::vector<FGlyphInstance> Instances;
	FGlyphAtlasInputLayout GlyphAtlasInputLayout;

	std::vector<FSimpleVertex> RectVertices = {
	{{0, 0}},
	{{1, 0}},
	{{1, 1}},
	{{0, 1}},
	};

	const std::vector<uint16_t> RectIndices = {
		0,3,2,2,1,0
	};
	VkExtent2D Extent = { 0, 0 };

	FRendering* Rendering = nullptr;
};