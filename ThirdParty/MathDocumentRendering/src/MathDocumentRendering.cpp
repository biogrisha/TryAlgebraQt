#include "MathDocumentRendering.h"
#include "MathDocumentRendering.h"
#include <GLFW/glfw3.h>
#include <stdexcept>
#include <iostream>
#include <MathLibrary.h>
#include <VulkanContext.h>
#include <chrono>
#include <algorithm>
void FMathDocumentRendering::Init()
{
	//Calculate dpi
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	int widthMM, heightMM;
	glfwGetMonitorPhysicalSize(monitor, &widthMM, &heightMM);
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);
	dpiX = (float)mode->width / (widthMM / 25.4f); 
	dpiY = (float)mode->height / (heightMM / 25.4f);

	//Init freetype library
	if (FT_Init_FreeType(&library)) {
		throw std::runtime_error("Could not init FreeType Library");
	}
	//Load font
	auto error = FT_New_Face(library,
		"C:/Windows/Fonts/Arial.ttf",
		0,
		&face);

	if (error == FT_Err_Unknown_File_Format)
	{
		throw std::runtime_error("FT_Err_Unknown_File_Format");
	}
	else if (error)
	{
		throw std::runtime_error("FontError");
	}

	Rendering = std::make_unique<FRendering>();

	AtlasRendering.Init(Rendering.get());
	TextFromAtlasRendering.SetAtlas(AtlasRendering.GetAtlas());
	TextFromAtlasRendering.Init(Rendering.get());
	Rendering->GetDescriptorManager().Init();
	AtlasRendering.InitPLine();
	TextFromAtlasRendering.InitPLine();
}

void FMathDocumentRendering::SetDocumentExtent(const VkExtent2D& InExtent)
{
	AtlasRendering.SetExtent(InExtent);
	TextFromAtlasRendering.SetExtent(InExtent);
	Extent = InExtent;
}

FGlyphRenderData FMathDocumentRendering::LoadGlyph(const FGlyphData& GlyphData)
{
	//Setting font size
	FGlyphRenderData Result;
	auto error = FT_Set_Char_Size(
		face,    /* handle to face object         */
		0,       /* char_width in 1/64 of points  */
		GlyphData.GlyphId.Height * DocumentScale * 64,   /* char_height in 1/64 of points */
		dpiX,     /* horizontal device resolution  */
		dpiY);

	//convert wchar into glyph index
	FT_ULong charcode = int(GlyphData.GlyphId.Glyph);
	auto glyph_index = FT_Get_Char_Index(face, charcode);

	//load glyph
	error = FT_Load_Glyph(
		face,          /* handle to face object */
		glyph_index,   /* glyph index           */
		0);


	//Decompose outline points into quadratic curves
	struct FOutlineDecomposeData
	{
		std::vector<FOutlineCurvePoints>* curvePoints = nullptr;
		int CurveStartIndex = 0;
	} data;
	data.curvePoints = &Result.Outline;

	FT_Outline_Funcs funcs;
	funcs.move_to = [](const FT_Vector* to, void* user) -> int
		{
			FOutlineDecomposeData* DecomposeData = (FOutlineDecomposeData*)user;
			if (!DecomposeData->curvePoints->empty())
			{
				auto& LastCurve = DecomposeData->curvePoints->back();
				auto& FirstCurve = (*DecomposeData->curvePoints)[DecomposeData->CurveStartIndex];
				LastCurve.points[2] = LastCurve.points[1] = { FirstCurve.points[0].x, FirstCurve.points[0].y };
			}
			DecomposeData->CurveStartIndex = (int)DecomposeData->curvePoints->size();
			auto& Curve = DecomposeData->curvePoints->emplace_back();
			Curve.points[0] = { to->x, to->y };
			return 0;
		};
	funcs.line_to = [](const FT_Vector* to, void* user) -> int
		{
			FOutlineDecomposeData* DecomposeData = (FOutlineDecomposeData*)user;
			auto& LastCurve = DecomposeData->curvePoints->back();
			LastCurve.points[2] = { to->x, to->y };
			LastCurve.points[1] = (LastCurve.points[2] + LastCurve.points[0]) / 2.f;
			auto LastCurveCopy = LastCurve;
			auto& NewCurve = DecomposeData->curvePoints->emplace_back();
			NewCurve.points[0] = LastCurveCopy.points[2];
			return 0;
		};
	funcs.conic_to = [](const FT_Vector* control, const FT_Vector* to, void* user) -> int
		{
			FOutlineDecomposeData* DecomposeData = (FOutlineDecomposeData*)user;
			auto& LastCurve = DecomposeData->curvePoints->back();
			LastCurve.points[2] = { to->x, to->y };
			LastCurve.points[1] = { control->x, control->y };
			auto LastCurveCopy = LastCurve;
			auto& NewCurve = DecomposeData->curvePoints->emplace_back();
			NewCurve.points[0] = LastCurveCopy.points[2];
			return 0;
		};
	funcs.cubic_to = [](const FT_Vector* c1, const FT_Vector* c2, const FT_Vector* to, void* user) -> int {
		// TODO: handle cubic Bézier
		return 0;
		};

	funcs.shift = 0;
	funcs.delta = 0;
	FT_Outline_Decompose(&face->glyph->outline, &funcs, &data);

	//Remove last curve since it is redundant due to how we decompose curves
	Result.Outline.pop_back();

	//Cache height and width of glyphs
	uint32_t Height = face->bbox.yMax - face->bbox.yMin;
	Result.HeightInPixels = Height / 64;
	Result.WidthInPixels = face->glyph->advance.x / 64;

	//Cache a and b of discriminant 
	for (auto& Curve : Result.Outline)
	{
		for (auto& Point : Curve.points)
		{
			Point.y = Height - Point.y + face->bbox.yMin;
		}
		std::swap(Curve.points[0], Curve.points[2]);
		Curve.a = Curve.points[0].y - 2 * Curve.points[1].y + Curve.points[2].y;
		Curve.b = Curve.points[0].y - Curve.points[1].y;
	}
	return Result;
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
			*RenderData = LoadGlyph(GlyphData);
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
	std::vector<FSpriteInstance> TextInstanceData;
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
	return TextFromAtlasRendering.Render();
}

bool FMathDocumentRendering::HasContent()
{
	return !DocumentContent.empty();
}

int32_t FMathDocumentRendering::GetGlyphAdvance(const FGlyphId& GlyphId)
{
	auto error = FT_Set_Char_Size(
		face,    /* handle to face object         */
		0,       /* char_width in 1/64 of points  */
		GlyphId.Height* DocumentScale * 64,   /* char_height in 1/64 of points */
		dpiX,     /* horizontal device resolution  */
		dpiY);

	//convert wchar into glyph index
	FT_ULong charcode = int(GlyphId.Glyph);
	auto glyph_index = FT_Get_Char_Index(face, charcode);

	//load glyph
	error = FT_Load_Glyph(
		face,          /* handle to face object */
		glyph_index,   /* glyph index           */
		0);

	return face->glyph->advance.x / 64;
}
