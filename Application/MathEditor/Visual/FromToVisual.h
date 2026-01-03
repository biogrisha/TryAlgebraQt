#pragma once
#include "MathDocumentRenderingStructs.h"
#include "Modules/Visual/VisualBase.h"
#include "Modules/MathElementsV2/Me/MeFromTo.h"
class FromToVisual : public FTAVisual
{
public:
	FromToVisual(MathElementV2::FTAMeFromTo* me, std::vector<FGlyphData>* glyphsPtr);
	virtual void Show() override;
	virtual TACommonTypes::FTAVector2d GetVisualSize();
private:
	std::vector<FGlyphData>* m_glyphsPtr = nullptr;
	MathElementV2::FTAMeFromTo* m_me = nullptr;
};