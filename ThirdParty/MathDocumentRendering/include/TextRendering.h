#pragma once
#include "GlyphAtlasRendering.h"
#include "TextFromAtlasRendering.h"
class FFreeTypeWrap;
class TextRendering
{

public:
	void init(FRendering* rendering, FImageBuffer* output, FFreeTypeWrap* ft);
	void initPLine();
	void updateText(std::vector<FGlyphData> documentContent);
	void render();
	void setExtent(const vk::Extent3D& extent);
private:
	FGlyphAtlasRendering m_atlasRendering;
	FTextFromAtlasRendering m_textFromAtlasRendering;
	FFreeTypeWrap* m_ft = nullptr;
	vk::Extent3D m_extent;
	bool m_hasContent = false;
};