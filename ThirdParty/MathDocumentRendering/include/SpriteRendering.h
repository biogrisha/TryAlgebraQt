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
	void Init(FRendering* InRendering, FImageBuffer* output);
	void InitPLine();
	void setExtent(const VkExtent3D& InExtent);
	void Render();
	void SetInstances(const std::vector<FSpriteInstByName>& Sprites);

	std::unique_ptr<FBuffer> VertexBuffer;
	std::unique_ptr<FBuffer> InstanceBuffer;
	std::unique_ptr<FBuffer> IndexBuffer;
	std::unique_ptr<FBuffer> UniformBuffer;
	FAtlas Atlas;
	FSpriteLayout SpriteLayout;
	uint16_t InstancesCount = 0;
	FImageBuffer* m_output = nullptr;
	std::vector<FVertTex> RectVertices = {
		{{0, 0},{0.0f, 0.0f}},
		{{1, 0},{1.0f, 0.0f}},
		{{1, 1},{1.0f, 1.0f}},
		{{0, 1},{0.0f, 1.0f}},
	};

	const std::vector<uint16_t> RectIndices = {
		0,3,2,2,1,0
	};
	VkExtent3D Extent = { 300,300,1 };

	FRendering* Rendering;
	uint16_t S_1, P_1, PLine = UINT16_MAX;
};