#pragma once
#include <map>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_OUTLINE_H
#include "MathDocumentRenderingStructs.h"

struct FGlyphRenderData
{
	std::vector<FOutlineCurvePoints> Outline;
	FT_Glyph_Metrics metrics;
	glm::vec2 TextureOffset = glm::vec2{ 0,0 };
};

struct FGlyphData
{
	FGlyphId GlyphId;
	glm::vec2 Pos = { 0,0 };
	FGlyphRenderData* RenderData = nullptr;
};

class FFreeTypeWrap
{
public:
	FFreeTypeWrap();
	~FFreeTypeWrap();
	void Init(float InDpiX, float InDpiY);
	void Shutdown();
	FGlyphRenderData LoadGlyph(const FGlyphId& GlyphId);
	FGlyphRenderData* GetGlyphRenderData(const FGlyphId& GlyphId);
	glm::vec2 GetGlyphSize(const FGlyphId& GlyphId);
	uint32_t GetHeightFromFontSize(float Points);
private:
	FT_Library  Library;
	FT_Face     Face;
	FT_Face     FaceFallback;
	float DpiX = 0;
	float DpiY = 0;
	std::map<FGlyphId, std::unique_ptr<FGlyphRenderData>> GlyphsRenderData;
};