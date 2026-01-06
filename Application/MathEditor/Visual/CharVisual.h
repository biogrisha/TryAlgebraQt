#pragma once
#include "Modules/Visual/VisualBase.h"
#include "Modules/MathElementsV2/Me/MeCharacter.h"
#include "MathDocumentState.h"

class CharVisual : public FTAVisual
{
public:
	CharVisual(MathElementV2::FTAMeCharacter* InMeChar, FMathDocumentState* meDocState);
	virtual void Show() override;
	virtual TACommonTypes::FTAVector2d GetVisualSize() override;
private:
	FMathDocumentState* m_meDocState = nullptr;
	MathElementV2::FTAMeCharacter* m_me = nullptr;
};