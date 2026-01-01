#pragma once
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include <vector>
#include "VertexInputLayout.h"

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

struct FVertTex
{
	glm::vec2 Pos;
	glm::vec2 TexCoord;
};

struct FGlyphSpriteInst
{
	glm::vec2 Pos;
	glm::vec2 Size;
	glm::vec2 TextureOffset;
};

struct FSpriteInstNamed
{
	std::string Name;
	glm::vec2 Pos;
	glm::vec2 Size;
};

struct FSpriteInst
{
	glm::ivec2 Pos;
	glm::ivec2 Size;
	glm::ivec2 TexPos;
	glm::ivec2 TexSize;
};
