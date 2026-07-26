#pragma once
#include "MathDocumentRenderingStructs.h"
#include "VertexInputLayout.h"
#include "Pipeline.h"
#include "DescriptorManager.h"
#include "Rendering.h"

class DrawImageLayout : public FVertexInputLayout {
	virtual std::vector<vk::VertexInputBindingDescription> getBindingDescription() override {
		return {
			{ 0, sizeof(FVertTex), vk::VertexInputRate::eVertex },
		};
	}
	virtual std::vector<vk::VertexInputAttributeDescription> getAttributeDescriptions() override {
		return {
			vk::VertexInputAttributeDescription(0, 0, vk::Format::eR32G32Sfloat, offsetof(FVertTex, Pos)),
			vk::VertexInputAttributeDescription(1, 0, vk::Format::eR32G32Sfloat, offsetof(FVertTex, TexCoord)),
		};
	}
};

class DrawImageRendering
{
public:
	void Init(FRendering* rendering, FImageBuffer* input, FImageBuffer* output);
	void InitPLine();
	void setExtent(const vk::Extent3D& extent);

	void Render(bool clearAttachment);
private:
	std::vector<FVertTex> m_rectVertices = {
		{{-1, -1},{0.0f, 0.0f}},
		{{1, -1},{1.0f, 0.0f}},
		{{1, 1},{1.0f, 1.0f}},
		{{-1, 1},{0.0f, 1.0f}},
	};

	const std::vector<uint16_t> m_rectIndices = {
		0,3,2,2,1,0
	};

	DrawImageLayout m_vertexLayout;
	std::unique_ptr<FBuffer> m_vertexBuffer;
	std::unique_ptr<FBuffer> m_indexBuffer;
	FImageBuffer* m_output = nullptr;

	VkExtent3D m_extent = { 300,300,1 };
	FRendering* m_rendering = nullptr;

	uint16_t SetHndl = UINT16_MAX;
	uint16_t PLineLayoutHndl = UINT16_MAX;
	uint16_t PLineHndl = UINT16_MAX;
};