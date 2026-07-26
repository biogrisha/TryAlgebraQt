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
#include "LinesRendering.h"
#include "TextRendering.h"
#include "DrawImageRendering.h"

class FMathDocumentRendering
{
public:
	FMathDocumentRendering();
	void Init(FFreeTypeWrap* ft);
	void SetDocumentExtent(const VkExtent3D& extent);
	FImageBuffer* Render();
	FMathDocumentState* getState();
private:


	VkExtent3D m_extent = { 0, 0, 1 };
	std::unique_ptr<FRendering> m_rendering;
	FMathDocumentState m_state;

	std::unique_ptr<FImageBuffer> m_output;
	//layer1
	std::unique_ptr<FImageBuffer> m_layer1;
	FRectRendering m_rectRendering1;

	//layer2
	std::unique_ptr<FImageBuffer> m_layer2;
	FSpriteRendering m_spriteRendering2;
	FRectRendering m_rectRendering2;
	LinesRendering m_linesRendering2;
	TextRendering m_textRendering2;

	DrawImageRendering m_layer1ToOutput;
	DrawImageRendering m_layer2ToOutput;
};