#pragma once
#include "MathDocumentRenderingStructs.h"
#include "Modules/Visual/VisualBase.h"
#include "Modules/MathElementsV2/Me/MeCharacter.h"

class CharVisual : public FTAVisual
{
public:
	CharVisual(MathElementV2::FTAMeCharacter* InMeChar, std::vector<FGlyphData>* InGlyphsPtr);
	virtual void Show() override;
private:
	std::vector<FGlyphData>* GlyphsPtr = nullptr;	
	MathElementV2::FTAMeCharacter* MeChar = nullptr;
};