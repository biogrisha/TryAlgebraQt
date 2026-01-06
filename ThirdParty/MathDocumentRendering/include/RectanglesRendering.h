#pragma once
#include "MathDocumentRenderingStructs.h"
#include "VertexInputLayout.h"
#include "Pipeline.h"
#include "DescriptorManager.h"
#include "Rendering.h"
#include "Atlas.h"

class FRectLayout : public FVertexInputLayout {
	virtual std::vector<vk::VertexInputBindingDescription> getBindingDescription() override {
		return {
			{ 0, sizeof(FVert), vk::VertexInputRate::eVertex },
			{ 1, sizeof(FRectInst), vk::VertexInputRate::eInstance },
		};
	}
	virtual std::vector<vk::VertexInputAttributeDescription> getAttributeDescriptions() override {
		return {
			vk::VertexInputAttributeDescription(0, 0, vk::Format::eR32G32Sfloat, offsetof(FVert, Pos)),

			vk::VertexInputAttributeDescription(1, 1, vk::Format::eR32G32Sint, offsetof(FRectInst, Pos)),
			vk::VertexInputAttributeDescription(2, 1, vk::Format::eR32G32Sint, offsetof(FRectInst, Size)),
			vk::VertexInputAttributeDescription(3, 1, vk::Format::eR32G32B32A32Sfloat, offsetof(FRectInst, Color)),
		};
	}
};

class FRectRendering
{
public:
	void Init(FRendering* InRendering);
	void InitPLine();
	void SetExtent(const VkExtent2D& InExtent);
	void Render();
	void SetInstances(const std::vector<FRectInst>& Rects);
	FImageBuffer* GetResult();
	bool HasInstances();

	std::unique_ptr<FBuffer> VertexBuffer;
	std::unique_ptr<FBuffer> InstanceBuffer;
	std::unique_ptr<FBuffer> IndexBuffer;
	std::unique_ptr<FBuffer> UniformBuffer;
	std::unique_ptr<FImageBuffer> Output;
	FRectLayout RectLayout;
	uint16_t InstancesCount = 0;

	std::vector<FVert> RectVertices = {
		{{0, 0}},
		{{1, 0}},
		{{1, 1}},
		{{0, 1}},
	};

	const std::vector<uint16_t> RectIndices = {
		0,3,2,2,1,0
	};
	VkExtent2D Extent = { 300,300 };
	FRendering* Rendering;
};