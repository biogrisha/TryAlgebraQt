#pragma once
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_OUTLINE_H
#include "MathDocumentRenderingStructs.h"

class FFreeTypeWrap
{
public:
	void Init(float InDpiX, float InDpiY);
	FGlyphRenderData LoadGlyph(const FGlyphData& GlyphData);
	glm::vec2 GetGlyphSize(const FGlyphId& GlyphId);
private:
	FT_Library  library;
	FT_Face     face;
	float DpiX = 0;
	float DpiY = 0;
};