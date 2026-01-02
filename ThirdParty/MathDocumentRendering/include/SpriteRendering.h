#pragma once
#include "MathDocumentRenderingStructs.h"
#include "VertexInputLayout.h"
#include "Pipeline.h"
#include "DescriptorManager.h"
#include "Rendering.h"
#include "Atlas.h"

class FSpriteLayout : public FVertexInputLayout {
	virtual std::vector<vk::VertexInputBindingDescription> getBindingDescription() override {
		return {
			{ 0, sizeof(FVertTex), vk::VertexInputRate::eVertex },
			{ 1, sizeof(FGlyphSpriteInst), vk::VertexInputRate::eInstance },
		};
	}
	virtual std::vector<vk::VertexInputAttributeDescription> getAttributeDescriptions() override {
		return {
			vk::VertexInputAttributeDescription(0, 0, vk::Format::eR32G32Sfloat, offsetof(FVertTex, Pos)),
			vk::VertexInputAttributeDescription(1, 0, vk::Format::eR32G32Sfloat, offsetof(FVertTex, TexCoord)),

			vk::VertexInputAttributeDescription(2, 1, vk::Format::eR32G32Sint, offsetof(FSpriteInst, Pos)),
			vk::VertexInputAttributeDescription(3, 1, vk::Format::eR32G32Sint, offsetof(FSpriteInst, Size)),
			vk::VertexInputAttributeDescription(4, 1, vk::Format::eR32G32Sint, offsetof(FSpriteInst, TexPos)),
			vk::VertexInputAttributeDescription(5, 1, vk::Format::eR32G32Sint, offsetof(FSpriteInst, TexSize)),
			vk::VertexInputAttributeDescription(6, 1, vk::Format::eR32Sfloat, offsetof(FSpriteInst, Alpha)),
		};
	}
};

class FSpriteRendering
{
public:
	void Init(FRendering* InRendering);
	void InitPLine();
	void SetExtent(const VkExtent2D& InExtent);
	void SetInput(FImageBuffer* InInput);
	void Render();
	void SetInstances(const std::vector<FSpriteInstByName>& Sprites);
	FImageBuffer* GetResult();

	std::unique_ptr<FBuffer> VertexBuffer;
	std::unique_ptr<FBuffer> InstanceBuffer;
	std::unique_ptr<FBuffer> IndexBuffer;
	std::unique_ptr<FBuffer> UniformBuffer;
	std::unique_ptr<FImageBuffer> Output;
	FAtlas Atlas;
	FImageBuffer* Input;
	FSpriteLayout SpriteLayout;
	uint16_t InstancesCount = 0;

	std::vector<FVertTex> RectVertices = {
		{{0, 0},{0.0f, 0.0f}},
		{{1, 0},{1.0f, 0.0f}},
		{{1, 1},{1.0f, 1.0f}},
		{{0, 1},{0.0f, 1.0f}},
	};

	const std::vector<uint16_t> RectIndices = {
		0,3,2,2,1,0
	};
	VkExtent2D Extent = { 300,300 };

	FRendering* Rendering;
};