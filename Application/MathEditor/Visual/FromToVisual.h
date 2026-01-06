#pragma once
#include "MathDocumentState.h"
#include "Modules/Visual/VisualBase.h"
#include "Modules/MathElementsV2/Me/MeFromTo.h"

class FromToVisual : public FTAVisual
{
public:
	FromToVisual(MathElementV2::FTAMeFromTo* me, FMathDocumentState* meDocState);
	virtual void Show() override;
	virtual TACommonTypes::FTAVector2d GetVisualSize();
private:
	FMathDocumentState* m_meDocState = nullptr;
	MathElementV2::FTAMeFromTo* m_me = nullptr;
};