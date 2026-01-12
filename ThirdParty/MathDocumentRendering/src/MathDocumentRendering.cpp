#include "MathDocumentRendering.h"
#include <stdexcept>
#include <iostream>
#include <MathLibrary.h>
#include <VulkanContext.h>
#include <chrono>
#include <algorithm>
#include "VulkanHelpers.h"

void FMathDocumentRendering::Init(FFreeTypeWrap* InFreeTypeWrap)
{
	FreeTypeWrap = InFreeTypeWrap;
	Rendering = std::make_unique<FRendering>();


	AtlasRendering.Init(Rendering.get());

	RectRendering.Init(Rendering.get());

	TextFromAtlasRendering.SetAtlas(AtlasRendering.GetAtlas());
	TextFromAtlasRendering.SetOutputImage(RectRendering.GetResult());
	TextFromAtlasRendering.Init(Rendering.get());

	SpriteRendering.SetInput(TextFromAtlasRendering.GetResultImage());
	SpriteRendering.Init(Rendering.get());

	Rendering->GetDescriptorManager().Init();

	AtlasRendering.InitPLine();
	RectRendering.InitPLine();
	TextFromAtlasRendering.InitPLine();
	SpriteRendering.InitPLine();
	
}

void FMathDocumentRendering::SetDocumentExtent(const VkExtent2D& InExtent)
{
	RectRendering.SetExtent(InExtent);
	AtlasRendering.SetExtent(InExtent);
	TextFromAtlasRendering.SetExtent(InExtent);
	SpriteRendering.SetExtent(InExtent);
	Extent = InExtent;
}


void FMathDocumentRendering::UpdateText(const std::vector<FGlyphData>& InDocumentContent)
{
	Atlas.clear();
	DocumentContent = InDocumentContent;
	if (InDocumentContent.empty())
	{
		return;
	}

	//for each glyph on the page
	for (auto& GlyphData : DocumentContent)
	{		
		GlyphData.RenderData = FreeTypeWrap->GetGlyphRenderData(GlyphData.GlyphId);
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

void FMathDocumentRendering::UpdateRects(const std::vector<FRectInst>& InRects)
{
	RectRendering.SetInstances(InRects);
}
void FMathDocumentRendering::UpdateCaret(const FCaretData& CaretData)
{
	FSpriteInstByName Caret;
	Caret.Alpha = 1;
	Caret.Pos = CaretData.Pos;
	Caret.Size = CaretData.Size;
	Caret.SpriteName = "TextCaret.png";
	SpriteRendering.SetInstances({ Caret });
}

void FMathDocumentRendering::UpdateState(const FMathDocumentState& NewState)
{
	State.CopyChanged(NewState);
}

FImageBuffer* FMathDocumentRendering::Render()
{
	if (State.IsRectsUpdated())
	{
		UpdateRects(State.GetRects());
	}
	if(State.IsTextUpdated())
	{
		UpdateText(State.GetText());
	}
	if (!HasContent())
	{
		auto CommandBuffer = VkHelpers::BeginSingleTimeCommands();
		VkHelpers::ClearImage(TextFromAtlasRendering.GetResultImage(), CommandBuffer);
		VkHelpers::EndSingleTimeCommands(CommandBuffer);
	}
	else if (State.IsRectsUpdated())
	{
		RectRendering.Render();
		if(State.IsTextUpdated())
		{
			AtlasRendering.Render();
		}
		TextFromAtlasRendering.Render(!RectRendering.HasInstances());
	}
	else if (State.IsTextUpdated())
	{
		RectRendering.Render();
		AtlasRendering.Render();
		TextFromAtlasRendering.Render(!RectRendering.HasInstances());
	}
	
	if (State.IsCaretUpdated())
	{
		UpdateCaret(State.GetCaretData());
	}
	SpriteRendering.Render();
	return SpriteRendering.GetResult();
}

bool FMathDocumentRendering::HasContent()
{
	return !DocumentContent.empty();
}
