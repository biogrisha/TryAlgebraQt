#pragma once
#include "VertexInputLayout.h"
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include <Pipeline.h>
#include <DescriptorManager.h>
#include <Rendering.h>

struct FSpriteVertex
{
	glm::vec2 Pos;
	glm::vec2 TexCoord;
};

struct FSpriteInstance
{
	glm::vec2 Pos;
	glm::vec2 Size;
	glm::vec2 TextureOffset;
};

class FTextFromAtlasLayout : public FVertexInputLayout {
	virtual std::vector<vk::VertexInputBindingDescription> getBindingDescription() override {
		return {
			{ 0, sizeof(FSpriteVertex), vk::VertexInputRate::eVertex },
			{ 1, sizeof(FSpriteInstance), vk::VertexInputRate::eInstance },
		};
	}
	virtual std::vector<vk::VertexInputAttributeDescription> getAttributeDescriptions() override {
		return {
			vk::VertexInputAttributeDescription(0, 0, vk::Format::eR32G32Sfloat, offsetof(FSpriteVertex, Pos)),
			vk::VertexInputAttributeDescription(1, 0, vk::Format::eR32G32Sfloat, offsetof(FSpriteVertex, TexCoord)),

			vk::VertexInputAttributeDescription(2, 1, vk::Format::eR32G32Sfloat, offsetof(FSpriteInstance, Pos)),
			vk::VertexInputAttributeDescription(3, 1, vk::Format::eR32G32Sfloat, offsetof(FSpriteInstance, Size)),
			vk::VertexInputAttributeDescription(4, 1, vk::Format::eR32G32Sfloat, offsetof(FSpriteInstance, TextureOffset)),
		};
	}
};

class FTextFromAtlasRendering
{
public:
	void Init(FRendering* InRendering);
	void InitPLine();
	void SetExtent(const VkExtent2D& InExtent);
	void SetAtlas(FImageBuffer* InAtlasBuffer);
	void SetInstances(const std::vector<FSpriteInstance>& InInstances);
	FImageBuffer* Render();

	std::unique_ptr<FBuffer> VertexBuffer;
	std::unique_ptr<FBuffer> InstanceBuffer;
	std::unique_ptr<FBuffer> IndexBuffer;
	std::unique_ptr<FBuffer> UniformBuffer;
	std::unique_ptr<FImageBuffer> Result;
	FTextFromAtlasLayout TextFromAtlasLayout;
	FImageBuffer* AtlasBuffer = nullptr;
	uint16_t InstancesCount = 0;

	std::vector<FSpriteVertex> RectVertices = {
	{{0, 0},{0.0f, 0.0f}},
	{{1, 0},{1.0f, 0.0f}},
	{{1, 1},{1.0f, 1.0f}},
	{{0, 1},{0.0f, 1.0f}},
	};

	const std::vector<uint16_t> RectIndices = {
		0,3,2,2,1,0
	};
	VkExtent2D Extent = {300,300};

	FRendering* Rendering;
};