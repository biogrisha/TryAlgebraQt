#pragma once
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_OUTLINE_H
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include <vector>

struct FOutlineCurvePoints
{
	glm::vec2 points[3];
	float a = 0;
	float b = 0;
};

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
	glm::vec2 Pos = { 0,0 };
	FGlyphRenderData* RenderData = nullptr;
};


class FFreeTypeWrap
{
public:
	void Init(float InDpiX, float InDpiY);
	FGlyphRenderData LoadGlyph(const FGlyphData& GlyphData);
	int32_t GetGlyphAdvance(const FGlyphId& GlyphId);
private:
	FT_Library  library;
	FT_Face     face;
	float DpiX = 0;
	float DpiY = 0;
};