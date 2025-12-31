#pragma once
#include "MathDocumentRenderingStructs.h"
#include "VertexInputLayout.h"
#include "Pipeline.h"
#include "DescriptorManager.h"
#include "Rendering.h"
#include "Atlas.h"

class FSpriteRendering
{
public:
	void Init(FRendering* InRendering);
	void InitPLine();
	void SetExtent(const VkExtent2D& InExtent);
	void SetInput(FImageBuffer* InInputTexture);
	void Render();
	FImageBuffer* GetResult();

	std::unique_ptr<FBuffer> VertexBuffer;
	std::unique_ptr<FBuffer> InstanceBuffer;
	std::unique_ptr<FBuffer> IndexBuffer;
	std::unique_ptr<FBuffer> UniformBuffer;
	FAtlas Atlas;
	FImageBuffer* InputTextrure;
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