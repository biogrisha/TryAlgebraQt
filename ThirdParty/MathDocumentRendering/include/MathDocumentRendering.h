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
#include "RectanglesRendering.h"
#include "MathDocumentState.h"
	
class FMathDocumentRendering
{
public:
	void Init(FFreeTypeWrap* InFreeTypeWrap);
	void SetDocumentExtent(const VkExtent2D& InExtent);
	void UpdateText(const std::vector<FGlyphData>& InDocumentContent);
	void UpdateRects(const std::vector<FRectInst>& InRects);
	void UpdateCaret(const FCaretData& caretData);
	void UpdateState(const FMathDocumentState& NewState);
	FImageBuffer* Render();
	bool HasContent();
private:
	std::vector<FGlyphData> DocumentContent;
	std::map<FGlyphId, FGlyphData> Atlas;
	FRectRendering RectRendering;
	FGlyphAtlasRendering AtlasRendering;
	FTextFromAtlasRendering TextFromAtlasRendering;
	FSpriteRendering SpriteRendering;

	VkExtent2D Extent = { 0, 0 };
	std::unique_ptr<FRendering> Rendering;
	FFreeTypeWrap* FreeTypeWrap = nullptr;

	FMathDocumentState State;
};