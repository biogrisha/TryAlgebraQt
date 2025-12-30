#include "CharVisual.h"

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
	Glyph.GlyphId.Height = 20;
	auto Pos = MeChar->GetAbsolutePosition();
	Glyph.Pos = { Pos.x, Pos.y };
	GlyphsPtr->push_back(Glyph);
}
