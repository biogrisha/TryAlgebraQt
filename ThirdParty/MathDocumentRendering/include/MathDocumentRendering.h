#pragma once
#include <cstdint>
#include <vector>
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include <map>
#include <memory>
#include <set>
#include <GlyphAtlasRendering.h>
#include <TextFromAtlasRendering.h>
#include <SpriteRendering.h>
	
class FMathDocumentRendering
{
public:
	void Init(FFreeTypeWrap* InFreeTypeWrap);
	void SetDocumentExtent(const VkExtent2D& InExtent);
	void SetDocumentContent(const std::vector<FGlyphData>& InDocumentContent);
	void UpdateCaret(const FCaretData& caretData);
	FImageBuffer* Render();
	bool HasContent();
private:
	std::vector<FGlyphData> DocumentContent;
	std::map<FGlyphId, FGlyphData> Atlas;
	std::map<FGlyphId, std::unique_ptr<FGlyphRenderData>> GlyphsRenderData;
	FGlyphAtlasRendering AtlasRendering;
	FTextFromAtlasRendering TextFromAtlasRendering;
	FSpriteRendering SpriteRendering;

	VkExtent2D Extent = { 0, 0 };
	std::unique_ptr<FRendering> Rendering;
	FFreeTypeWrap* FreeTypeWrap = nullptr;

	bool bUpdatedText = false;
};