#pragma once
#include <Modules/Visual/VisualBase.h>
#include <Modules/MathElementsV2/Me/MathElementBaseV2.h>
#include <MathDocumentState.h>


class MeVisualBase : public FTAVisual
{
public:
	MeVisualBase(FMathDocumentState* meDocState);
	virtual void Show(MathElementV2::FTAMathElementBase* me) override;
protected:
	FMathDocumentState* m_meDocState = nullptr;
};