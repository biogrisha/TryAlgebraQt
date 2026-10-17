#pragma once
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include <vector>
#include "VertexInputLayout.h"

struct FOutlineCurvePoints
{
	glm::vec2 points[3] = { glm::vec2(),glm::vec2(),glm::vec2() };
	//discreminant a and b coeffs
	float a = 0;
	float b = 0;
};


struct OutlineBuilder
{

	struct Point
	{
		glm::vec2 pos;
		bool isControl = false;
	};

	void addPoint(float x, float y, bool control = false)
	{
		if (rings.empty())
		{
			rings.emplace_back();
		}
		rings.back().push_back(Point{ .pos = {x,y}, .isControl = control });
		if (rings.back().size() > 1 && rings.back().back().pos == rings.back().front().pos)
		{
			rings.emplace_back();
		}
	}
	std::vector<FOutlineCurvePoints> outline()
	{
		std::vector<FOutlineCurvePoints> res;

		auto midpoint = [](glm::vec2 a, glm::vec2 b) {
			return (a + b) * 0.5f;
			};

		auto addCurve = [&](glm::vec2 a, glm::vec2 b, glm::vec2 c) {
			res.push_back(FOutlineCurvePoints{ .points = {a, b, c} });
			};

		for (const auto& ring : rings)
		{
			if (ring.size() < 2)
				continue;

			size_t n = ring.size();

			// Ignore duplicated closing point.
			if (ring.front().pos == ring.back().pos)
				--n;

			if (n < 2)
				continue;

			// Determine starting on-curve point.
			glm::vec2 start;

			if (!ring[0].isControl)
			{
				start = ring[0].pos;
			}
			else if (!ring[n - 1].isControl)
			{
				start = ring[n - 1].pos;
			}
			else
			{
				start = midpoint(ring[n - 1].pos, ring[0].pos);
			}

			glm::vec2 current = start;

			for (size_t i = 0; i < n; ++i)
			{
				const auto& p = ring[i];
				const auto& next = ring[(i + 1) % n];

				if (!p.isControl)
				{
					if (!next.isControl)
					{
						// Straight line.
						addCurve(p.pos, midpoint(p.pos, next.pos), next.pos);
						current = next.pos;
					}
				}
				else
				{
					// Quadratic curve.
					glm::vec2 end = next.isControl
						? midpoint(p.pos, next.pos)
						: next.pos;

					addCurve(current, p.pos, end);
					current = end;
				}
			}
		}

		return res;
	}

	std::vector<std::vector<Point>> rings;
};


struct FGlyphInstance
{
	glm::vec2 Offset;
	glm::vec2 Size;
	uint32_t StartIndex;
	uint32_t CurvesCount;
};


struct FGlyphRenderData
{
	std::vector<FOutlineCurvePoints> Outline;
	uint32_t HeightInPixels = 0;
	uint32_t WidthInPixels = 0;
	glm::vec2 TextureOffset = glm::vec2{ 0,0 };
};

struct FGlyphId
{
	wchar_t Glyph{};
	uint16_t Height = 10;
	bool bCompact = false;

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

struct FVert
{
	glm::vec2 Pos;
};

struct FVertColored
{
	glm::vec2 Pos;
	glm::vec4 Color;
};

struct FGlyphSpriteInst
{
	glm::vec2 Pos;
	glm::vec2 Size;
	glm::vec2 TextureOffset;
};

struct FSpriteInstByName
{
	glm::ivec2 Pos;
	glm::ivec2 Size;
	std::string SpriteName;
	float Alpha = 0;
};

struct FSpriteInst
{
	glm::ivec2	Pos;
	glm::ivec2	Size;
	glm::ivec2	TexPos;
	glm::ivec2	TexSize;
	float		Alpha = 0;
};

struct FCaretData
{
	glm::vec2 Pos;
	glm::vec2 Size;
};

struct FRectInst
{
	glm::ivec2 Pos;
	glm::ivec2 Size;
	glm::vec4 Color;
};

struct LineChain
{
	float width = 1;
	std::vector<glm::vec2> points;
	glm::vec4 color;
};
