#include "FromToVisual.h"
#include "FreeTypeWrap.h"
#include "Application.h"
#include "AppGlobal.h"

FromToVisual::FromToVisual(MathElementV2::FTAMeFromTo* me, std::vector<FGlyphData>* glyphsPtr)
{
	m_glyphsPtr = glyphsPtr;
	m_me = me;
}

void FromToVisual::Show()
{
	auto pos = m_me->GetAbsolutePosition() + m_me->GetSymbolPosition();
	FGlyphData g;
	g.GlyphId.Glyph = m_me->GetSymbol().back();
	g.GlyphId.Height = m_me->GetFontSize();
	g.Pos = { pos.x, pos.y };
	m_glyphsPtr->push_back(g);
}

TACommonTypes::FTAVector2d FromToVisual::GetVisualSize()
{
	auto ft = AppGlobal::application->getFreeTypeWrap();
	FGlyphId Glyph;
	Glyph.Glyph = m_me->GetSymbol().back();
	Glyph.Height = m_me->GetFontSize();
	auto Size = ft->GetGlyphSize(Glyph);
	return { Size.x, Size.y };
}

