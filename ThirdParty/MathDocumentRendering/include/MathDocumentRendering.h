#pragma once
#include <cstdint>
#include <vector>
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include <ft2build.h>
#include <map>
#include <memory>
#include <set>
#include FT_FREETYPE_H
#include FT_OUTLINE_H
#include <GlyphAtlasRendering.h>
#include <TextFromAtlasRendering.h>

struct FGlyphRenderData
{
	std::vector<FOutlineCurvePoints> Outline;
	uint32_t HeightInPixels = 0;
	uint32_t WidthInPixels = 0;
	glm::vec2 TextureOffset;
};

struct FGlyphId
{
	wchar_t Glyph{};
	uint16_t Height = 10;

	bool operator < (const FGlyphId& Other) const
	{
		if (Glyph == Other.Glyph)
		{
			return Height < Other.Height;
		}
		return Glyph < Other.Glyph;
	}
};

struct FGlyphData
{
	FGlyphId GlyphId;
	glm::vec2 Pos = {0,0};
	FGlyphRenderData* RenderData = nullptr;
};

	
class FMathDocumentRendering
{
public:
	void Init();
	void SetDocumentExtent(const VkExtent2D& InExtent);
	FGlyphRenderData LoadGlyph(const FGlyphData& GlyphData);
	void SetDocumentContent(const std::vector<FGlyphData>& InDocumentContent);
	FImageBuffer* Render();
	bool HasContent();
	int32_t GetGlyphAdvance(const FGlyphId& GlyphId);
private:
	std::vector<FGlyphData> DocumentContent;
	std::map<FGlyphId, FGlyphData> Atlas;
	std::map<FGlyphId, std::unique_ptr<FGlyphRenderData>> GlyphsRenderData;
	FT_Library  library;
	FT_Face     face;
	float dpiX = 0;
	float dpiY = 0;
	float DocumentScale = 1;
	FGlyphAtlasRendering AtlasRendering;
	FTextFromAtlasRendering TextFromAtlasRendering;
	VkExtent2D Extent = { 0, 0 };
	std::unique_ptr<FRendering> Rendering;
};