#include "CharVisual.h"
#include <Modules/MathElementsV2/Me/MeCharacter.h>
#include <FreeTypeWrap.h>
#include <Application.h>
#include <AppGlobal.h>

CharVisual::CharVisual(FMathDocumentState* meDocState)
	:MeVisualBase(meDocState)
{

}

void CharVisual::Show(MathElementV2::FTAMathElementBase* me)
{
	//cast to char
	auto meChar = me->Cast<MathElementV2::FTAMeCharacter>();
	auto ch = meChar->GetChar();
	auto pos = meChar->GetAbsolutePosition();
	auto size = meChar->GetAbsoluteSize();
	if (m_meDocState->IsTextUpdated() && ch != L' ')
	{
		//If this is character that could be displayed
		FGlyphData glyph;
		glyph.GlyphId.Glyph = ch;
		glyph.GlyphId.Height = meChar->GetFontSize();
		glyph.Pos = { pos.x, pos.y };
		//Append its parameters to glyphs array
		m_meDocState->AddGlyph(glyph);
	}
	MeVisualBase::Show(me);
}

TACommonTypes::FTAVector2d CharVisual::GetVisualSize(MathElementV2::FTAMathElementBase* me)
{
	auto meChar = me->Cast<MathElementV2::FTAMeCharacter>();
	auto ft = AppGlobal::application->getFreeTypeWrap();
	FGlyphId glyph;
	glyph.Glyph = meChar->GetChar();
	if (glyph.Glyph == L' ')
	{
		//if symbol can't be displayed by freetype -> use size of M
		glyph.Glyph = L'M';
	}
	//return size from freetype module for the given font size
	glyph.Height = meChar->GetFontSize();
	auto size = ft->GetGlyphSize(glyph);
	return { size.x, size.y };
}
