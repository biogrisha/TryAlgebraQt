#pragma once
#include <Modules/Visual/VisualBase.h>
#include <MathDocumentState.h>

class FTACursorComponent;

/*
* Collects visual data for caret
*/
class CaretVisual : public FTAVisual
{
public:
	CaretVisual(FTACursorComponent* cursorComp, FMathDocumentState* meDocState);
	virtual void Show() override;
private:
	FTACursorComponent* m_cursorComp = nullptr;
	FMathDocumentState* m_meDocState = nullptr;
};