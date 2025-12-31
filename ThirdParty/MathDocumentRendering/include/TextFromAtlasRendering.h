#pragma once
#include "VertexInputLayout.h"
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include <Pipeline.h>
#include <DescriptorManager.h>
#include "MathDocumentRenderingStructs.h"
#include <Rendering.h>

class FTextFromAtlasRendering
{
public:
	void Init(FRendering* InRendering);
	void InitPLine();
	void SetExtent(const VkExtent2D& InExtent);
	void SetAtlas(FImageBuffer* InAtlasBuffer);
	void SetInstances(const std::vector<FGlyphSpriteInst>& InInstances);
	FImageBuffer* GetResultImage();
	void Render();

	std::unique_ptr<FBuffer> VertexBuffer;
	std::unique_ptr<FBuffer> InstanceBuffer;
	std::unique_ptr<FBuffer> IndexBuffer;
	std::unique_ptr<FBuffer> UniformBuffer;
	std::unique_ptr<FImageBuffer> Result;
	FGlyphSpriteLayout TextFromAtlasLayout;
	FImageBuffer* AtlasBuffer = nullptr;
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
	VkExtent2D Extent = {300,300};

	FRendering* Rendering;
};