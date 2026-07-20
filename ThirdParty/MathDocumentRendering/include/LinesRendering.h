#pragma once
#include "MathDocumentRenderingStructs.h"
#include "VertexInputLayout.h"
#include "Pipeline.h"
#include "DescriptorManager.h"
#include "Rendering.h"
#include "Atlas.h"

class FLineLayout : public FVertexInputLayout {
	virtual std::vector<vk::VertexInputBindingDescription> getBindingDescription() override {
		return {
			{ 0, sizeof(FVertColored), vk::VertexInputRate::eVertex },
		};
	}
	virtual std::vector<vk::VertexInputAttributeDescription> getAttributeDescriptions() override {
		return {
			vk::VertexInputAttributeDescription(0, 0, vk::Format::eR32G32Sfloat, offsetof(FVertColored, Pos)),
			vk::VertexInputAttributeDescription(1, 0, vk::Format::eR32G32B32A32Sfloat, offsetof(FVertColored, Color)),
		};
	}
};

struct LinesChain
{
	float width = 1;
	std::vector<glm::vec2> points;
	glm::vec4 color;
};

class LinesRendering
{
public:
	void Init(FRendering* InRendering, FImageBuffer* InOutput);
	void InitPLine();
	void SetExtent(const VkExtent3D& InExtent);

	void Render();
	FImageBuffer* GetResult();
	void addChain(LinesChain chain);
	void flushIntoBuffer();

	std::unique_ptr<FBuffer> m_vertexBuffer;
	std::unique_ptr<FBuffer> m_indexBuffer;
	std::unique_ptr<FBuffer> m_uniformBuffer;
	FImageBuffer* m_output = nullptr;
	FLineLayout m_lineLayout;

	VkExtent3D m_extent = { 300,300,1 };
	FRendering* m_rendering = nullptr;
	std::vector<LinesChain> m_chains;
};