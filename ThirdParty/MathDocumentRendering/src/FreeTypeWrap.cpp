#include "FreeTypeWrap.h"
#include <GLFW/glfw3.h>
#include <stdexcept>

void FFreeTypeWrap::Init(float InDpiX, float InDpiY)
{
	
	DpiX = InDpiX;
	DpiY = InDpiY;

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
}

FGlyphRenderData FFreeTypeWrap::LoadGlyph(const FGlyphData& GlyphData)
{
	//Setting font size
	FGlyphRenderData Result;
	auto error = FT_Set_Char_Size(
		face,    /* handle to face object         */
		0,       /* char_width in 1/64 of points  */
		GlyphData.GlyphId.Height * 64,   /* char_height in 1/64 of points */
		DpiX,     /* horizontal device resolution  */
		DpiY);

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

inline int32_t FFreeTypeWrap::GetGlyphAdvance(const FGlyphId& GlyphId)
{
	auto error = FT_Set_Char_Size(
		face,    /* handle to face object         */
		0,       /* char_width in 1/64 of points  */
		GlyphId.Height * 64,   /* char_height in 1/64 of points */
		DpiX,     /* horizontal device resolution  */
		DpiY);

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
