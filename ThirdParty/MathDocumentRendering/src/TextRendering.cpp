#include "TextRendering.h"
#include "FreeTypeWrap.h"
void TextRendering::init(FRendering* rendering, FImageBuffer* output, FFreeTypeWrap* ft)
{
	m_ft = ft;
	m_atlasRendering.Init(rendering);

	m_textFromAtlasRendering.SetAtlas(m_atlasRendering.GetAtlas());
	m_textFromAtlasRendering.SetOutputImage(output);
	m_textFromAtlasRendering.Init(rendering);
}

void TextRendering::initPLine()
{
	m_atlasRendering.InitPLine();
	m_textFromAtlasRendering.InitPLine();
}

void TextRendering::updateText(std::vector<FGlyphData> documentContent)
{
	m_hasContent = !documentContent.empty();
	if (!m_hasContent)
	{
		return;
	}
	std::map<FGlyphId, FGlyphData> atlasContent;
	//for each glyph on the page
	for (auto& glyphData : documentContent)
	{
		glyphData.RenderData = m_ft->GetGlyphRenderData(glyphData.GlyphId);
		//Add unique glyph/size into atlas
		atlasContent.emplace(glyphData.GlyphId, glyphData);
	}
	//create instance data and outline array to pass into shader
	std::vector<FOutlineCurvePoints> outlineData;
	std::vector<FGlyphInstance> atlasInstanceData(atlasContent.size());

	uint32_t maxX = m_extent.width;
	uint32_t maxY = m_extent.height;
	uint32_t currX = 0;
	uint32_t currY = 0;
	uint32_t maxGlyphY = 0;

	int glyphId = 0;
	int curveId = 0;

	for (auto& glyphData : atlasContent)
	{

		if (currX + glyphData.second.RenderData->WidthInPixels > maxX)
		{
			currX = 0;
			currY += maxGlyphY;
		}
		glyphData.second.RenderData->TextureOffset.x = currX;
		glyphData.second.RenderData->TextureOffset.y = currY;

		atlasInstanceData[glyphId].Offset = glm::vec2(currX, currY);
		atlasInstanceData[glyphId].Size = glm::vec2(glyphData.second.RenderData->WidthInPixels, glyphData.second.RenderData->HeightInPixels);

		currX += glyphData.second.RenderData->WidthInPixels;
		maxGlyphY = std::max(maxGlyphY, glyphData.second.RenderData->HeightInPixels);
		auto& outline = glyphData.second.RenderData->Outline;
		atlasInstanceData[glyphId].StartIndex = curveId;
		atlasInstanceData[glyphId].CurvesCount = outline.size();
		outlineData.insert(outlineData.end(), outline.begin(), outline.end());

		curveId += outline.size();
		glyphId++;
	}

	//Set rendering data in atlas renderer
	m_atlasRendering.SetInstances(atlasInstanceData);
	m_atlasRendering.SetOutlineCurves(outlineData);

	//Set instance data for text renderer
	std::vector<FGlyphSpriteInst> textInstanceData;
	for (auto& glyphData : documentContent)
	{
		auto& spriteInstance = textInstanceData.emplace_back();
		spriteInstance.Pos = glyphData.Pos;
		spriteInstance.Size = glm::vec2{ glyphData.RenderData->WidthInPixels, glyphData.RenderData->HeightInPixels };
		spriteInstance.TextureOffset = glyphData.RenderData->TextureOffset;
	}
	m_textFromAtlasRendering.SetInstances(textInstanceData);
}

void TextRendering::render()
{
	if (!m_hasContent)
	{
		return;
	}
	m_atlasRendering.Render();
	m_textFromAtlasRendering.Render();
}

void TextRendering::setExtent(const vk::Extent3D& extent)
{
	m_extent = extent;
	m_atlasRendering.SetExtent(extent);
	m_textFromAtlasRendering.SetExtent(extent);
}