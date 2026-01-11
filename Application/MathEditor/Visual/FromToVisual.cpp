#include "FromToVisual.h"
#include <FreeTypeWrap.h>
#include <Application.h>
#include <AppGlobal.h>

FromToVisual::FromToVisual(MathElementV2::FTAMeFromTo* me, FMathDocumentState* meDocState)
{
	m_me = me;
	m_meDocState = meDocState;
}

void FromToVisual::Show()
{
	if (m_meDocState->IsTextUpdated())
	{
		//Add symbol parameters to glyphs render data
		auto symbPos = m_me->GetAbsolutePosition() + m_me->GetSymbolPosition();
		FGlyphData g;
		g.GlyphId.Glyph = m_me->GetSymbol().back();
		g.GlyphId.Height = m_me->GetFontSize();
		g.GlyphId.bCompact = true;
		g.Pos = { symbPos.x, symbPos.y };
		m_meDocState->AddGlyph(g);
	}
	if (m_meDocState->IsRectsUpdated())
	{
		if(m_me->bSelected)
		{
			//if me is selected -> add rectangle for the highlighting
			auto pos = m_me->GetAbsolutePosition();
			auto size = m_me->GetAbsoluteSize();
			m_meDocState->AddRect(FRectInst({ pos.x, pos.y }, { size.x, size.y }, { 0,0,0.8,1 }));
		}
	}
	FTAVisual::Show();
}

TACommonTypes::FTAVector2d FromToVisual::GetVisualSize()
{
	//Calculates Symbol size in pixels for the given font size from the outlines bounding box
	auto ft = AppGlobal::application->getFreeTypeWrap();
	FGlyphId g;
	g.Glyph = m_me->GetSymbol().back();
	g.Height = m_me->GetFontSize();
	g.bCompact = true;
	auto size = ft->GetGlyphSize(g);
	return { size.x, size.y };
}

