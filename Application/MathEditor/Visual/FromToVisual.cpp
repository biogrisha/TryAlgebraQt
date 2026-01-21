#include "FromToVisual.h"
#include <Modules/MathElementsV2/Me/MeFromTo.h>
#include <FreeTypeWrap.h>
#include <Application.h>
#include <AppGlobal.h>

FromToVisual::FromToVisual(FMathDocumentState* meDocState)
	:MeVisualBase(meDocState)
{
}

void FromToVisual::Show(MathElementV2::FTAMathElementBase* me)
{
	if (m_meDocState->IsTextUpdated())
	{
		auto meFt = me->Cast<MathElementV2::FTAMeFromTo>();
		//Add symbol parameters to glyphs render data
		auto symbPos = meFt->GetAbsolutePosition() + meFt->GetSymbolPosition();
		FGlyphData g;
		g.GlyphId.Glyph = meFt->GetSymbol().back();
		g.GlyphId.Height = meFt->GetFontSize();
		g.GlyphId.bCompact = true;
		g.Pos = { symbPos.x, symbPos.y };
		m_meDocState->AddGlyph(g);
	}
	FTAVisual::Show(me);
}

TACommonTypes::FTAVector2d FromToVisual::GetVisualSize(MathElementV2::FTAMathElementBase* me)
{
	auto meFt = me->Cast<MathElementV2::FTAMeFromTo>();
	//Calculates Symbol size in pixels for the given font size from the outlines bounding box
	auto ft = AppGlobal::application->getFreeTypeWrap();
	FGlyphId g;
	g.Glyph = meFt->GetSymbol().back();
	g.Height = meFt->GetFontSize();
	g.bCompact = true;
	auto size = ft->GetGlyphSize(g);
	return { size.x, size.y };
}

