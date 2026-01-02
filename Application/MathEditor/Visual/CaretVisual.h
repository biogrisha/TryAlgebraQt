#pragma once
#include "Modules/Visual/VisualBase.h"
#include "MathDocumentRenderingStructs.h"

class FTACursorComponent;

class CaretVisual : public FTAVisual
{
public:
	CaretVisual(FTACursorComponent* cursorComp, FCaretData* caretDataPtr);
	virtual void Show() override;
private:
	FTACursorComponent* m_cursorComp = nullptr;
	FCaretData* m_caretDataPtr = nullptr;
};