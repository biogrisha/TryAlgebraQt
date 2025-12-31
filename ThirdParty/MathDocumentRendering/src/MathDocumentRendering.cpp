#include "MathDocumentRendering.h"
#include <stdexcept>
#include <iostream>
#include <MathLibrary.h>
#include <VulkanContext.h>
#include <chrono>
#include <algorithm>
void FMathDocumentRendering::Init(FFreeTypeWrap* InFreeTypeWrap)
{
	FreeTypeWrap = InFreeTypeWrap;
	Rendering = std::make_unique<FRendering>();

	AtlasRendering.Init(Rendering.get());
	TextFromAtlasRendering.SetAtlas(AtlasRendering.GetAtlas());
	TextFromAtlasRendering.Init(Rendering.get());
	SpriteRendering.SetInput(TextFromAtlasRendering.GetResultImage());
	SpriteRendering.Init(Rendering.get());

	Rendering->GetDescriptorManager().Init();

	AtlasRendering.InitPLine();
	TextFromAtlasRendering.InitPLine();
	SpriteRendering.InitPLine();
	
}

void FMathDocumentRendering::SetDocumentExtent(const VkExtent2D& InExtent)
{
	AtlasRendering.SetExtent(InExtent);
	TextFromAtlasRendering.SetExtent(InExtent);
	SpriteRendering.SetExtent(InExtent);
	Extent = InExtent;
}


void FMathDocumentRendering::SetDocumentContent(const std::vector<FGlyphData>& InDocumentContent)
{
	if (InDocumentContent.empty())
	{
		return;
	}

	Atlas.clear();
	DocumentContent = InDocumentContent;
	//for each glyph on the page
	for (auto& GlyphData : DocumentContent)
	{
		//find render data in cache
		auto It = GlyphsRenderData.find(GlyphData.GlyphId);
		if (It != GlyphsRenderData.end())
		{
			//If found -> cache it for glyph
			GlyphData.RenderData = It->second.get();
		}
		else
		{
			//if no render data -> make new
			std::unique_ptr<FGlyphRenderData> RenderData = std::make_unique<FGlyphRenderData>();
			GlyphData.RenderData = RenderData.get();
			//Load render data
			*RenderData = FreeTypeWrap->LoadGlyph(GlyphData);
			//Add to render data cache
			GlyphsRenderData.emplace(GlyphData.GlyphId, std::move(RenderData));
		}
		//Add unique glyph/size into atlas
		Atlas.emplace(GlyphData.GlyphId, GlyphData);
	}
	//create instance data and outline array to pass into shader
	std::vector<FOutlineCurvePoints> OutlineData;
	std::vector<FGlyphInstance> AtlasInstanceData(Atlas.size());

	uint32_t MaxX = Extent.width;
	uint32_t MaxY = Extent.height;
	uint32_t CurrX = 0;
	uint32_t CurrY = 0;
	uint32_t MaxGlyphY = 0;

	int GlyphId = 0;
	int CurveId = 0;
	
	for (auto& GlyphData : Atlas)
	{
		
		if (CurrX + GlyphData.second.RenderData->WidthInPixels > MaxX)
		{
			CurrX = 0;
			CurrY += MaxGlyphY;
		}
		GlyphData.second.RenderData->TextureOffset.x = CurrX;
		GlyphData.second.RenderData->TextureOffset.y = CurrY;

		AtlasInstanceData[GlyphId].Offset = glm::vec2(CurrX, CurrY);
		AtlasInstanceData[GlyphId].Size = glm::vec2(GlyphData.second.RenderData->WidthInPixels, GlyphData.second.RenderData->HeightInPixels);
		
		CurrX += GlyphData.second.RenderData->WidthInPixels;
		MaxGlyphY = std::max(MaxGlyphY, GlyphData.second.RenderData->HeightInPixels);
		auto& Outline = GlyphData.second.RenderData->Outline;
		AtlasInstanceData[GlyphId].StartIndex = CurveId;
		AtlasInstanceData[GlyphId].CurvesCount = Outline.size();
		OutlineData.insert(OutlineData.end(), Outline.begin(), Outline.end());

		CurveId += Outline.size();
		GlyphId++;
	}

	//Set rendering data in atlas renderer
	AtlasRendering.SetInstances(AtlasInstanceData);
	AtlasRendering.SetOutlineCurves(OutlineData);
	
	//Set instance data for text renderer
	std::vector<FGlyphSpriteInst> TextInstanceData;
	for (auto& GlyphData : DocumentContent)
	{
		auto& SpriteInstance = TextInstanceData.emplace_back();
		SpriteInstance.Pos = GlyphData.Pos;
		SpriteInstance.Size = glm::vec2{ GlyphData.RenderData->WidthInPixels, GlyphData.RenderData->HeightInPixels };
		SpriteInstance.TextureOffset = GlyphData.RenderData->TextureOffset;
	}
	TextFromAtlasRendering.SetInstances(TextInstanceData);

}

FImageBuffer* FMathDocumentRendering::Render()
{
	AtlasRendering.Render();
	TextFromAtlasRendering.Render();
	SpriteRendering.Render();
	return SpriteRendering.GetResult();
}

bool FMathDocumentRendering::HasContent()
{
	return !DocumentContent.empty();
}
