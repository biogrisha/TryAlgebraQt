#pragma once
#include <MathEditor/Visual/MeVisualBase.h>

/*
* Collects visual info for characters
*/
class CharVisual : public MeVisualBase
{
public:
	CharVisual(FMathDocumentState* meDocState);
	virtual void Show(MathElementV2::FTAMathElementBase* me) override;
	virtual TACommonTypes::FTAVector2d GetVisualSize(MathElementV2::FTAMathElementBase* me) override;
};