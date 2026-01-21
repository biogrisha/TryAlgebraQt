#pragma once
#include <MathEditor/Visual/MeVisualBase.h>

/*
* Collects vilual data from MeFromTo
*/
class FromToVisual : public MeVisualBase
{
public:
	FromToVisual(FMathDocumentState* meDocState);
	virtual void Show(MathElementV2::FTAMathElementBase* me) override;
	virtual TACommonTypes::FTAVector2d GetVisualSize(MathElementV2::FTAMathElementBase* me);
};