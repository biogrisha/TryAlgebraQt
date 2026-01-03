#include "CharVisual.h"
#include "FreeTypeWrap.h"
#include "Application.h"
#include "AppGlobal.h"

CharVisual::CharVisual(MathElementV2::FTAMeCharacter* InMeChar, std::vector<FGlyphData>* InGlyphsPtr)
{
	MeChar = InMeChar;
	GlyphsPtr = InGlyphsPtr;
}

void CharVisual::Show()
{
	auto Char = MeChar->GetChar();
	if (Char == L' ')
	{
		return;
	}
	FGlyphData Glyph;
	Glyph.GlyphId.Glyph = Char;
	Glyph.GlyphId.Height = MeChar->GetFontSize();
	auto Pos = MeChar->GetAbsolutePosition();
	Glyph.Pos = { Pos.x, Pos.y };
	GlyphsPtr->push_back(Glyph);
	FTAVisual::Show();
}

TACommonTypes::FTAVector2d CharVisual::GetVisualSize()
{
	auto ft = AppGlobal::application->getFreeTypeWrap();
	FGlyphId Glyph;
	Glyph.Glyph = MeChar->GetChar();
	if (Glyph.Glyph == L' ')
	{
		Glyph.Glyph = L'M';
	}
	Glyph.Height = MeChar->GetFontSize();
	auto Size = ft->GetGlyphSize(Glyph);
	return { Size.x, Size.y };
}
