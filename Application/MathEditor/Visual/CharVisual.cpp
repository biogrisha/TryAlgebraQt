#include "CharVisual.h"
#include <FreeTypeWrap.h>
#include <Application.h>
#include <AppGlobal.h>

CharVisual::CharVisual(MathElementV2::FTAMeCharacter* InMeChar, FMathDocumentState* meDocState)
{
	m_me = InMeChar;
	m_meDocState = meDocState;
}

void CharVisual::Show()
{
	auto ch = m_me->GetChar();
	auto pos = m_me->GetAbsolutePosition();
	auto size = m_me->GetAbsoluteSize();
	if (m_meDocState->IsTextUpdated() && ch != L' ')
	{
		//If this is character that could be displayed
		FGlyphData glyph;
		glyph.GlyphId.Glyph = ch;
		glyph.GlyphId.Height = m_me->GetFontSize();
		glyph.Pos = { pos.x, pos.y };
		//Append its parameters to glyphs array
		m_meDocState->AddGlyph(glyph);
	}
	if (m_meDocState->IsRectsUpdated())
	{
		//Highlighting was updated
		if(m_me->bSelected)
		{
			//Me is selected->add highlight rect
			m_meDocState->AddRect(FRectInst({ pos.x, pos.y }, { size.x, size.y }, { 0,0,0.8,1 }));
		}
	}
	FTAVisual::Show();
}

TACommonTypes::FTAVector2d CharVisual::GetVisualSize()
{
	auto ft = AppGlobal::application->getFreeTypeWrap();
	FGlyphId glyph;
	glyph.Glyph = m_me->GetChar();
	if (glyph.Glyph == L' ')
	{
		//if symbol can't be displayed by freetype -> use size of M
		glyph.Glyph = L'M';
	}
	//return size from freetype module for the given font size
	glyph.Height = m_me->GetFontSize();
	auto size = ft->GetGlyphSize(glyph);
	return { size.x, size.y };
}
