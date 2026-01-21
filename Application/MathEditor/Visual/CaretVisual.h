#pragma once
#include <Modules/Visual/CursorVisualBase.h>
#include <MathDocumentState.h>

class FTACursorComponent;

/*
* Collects visual data for caret
*/
class CaretVisual : public FTACursorVisualBase
{
public:
	CaretVisual(FTACursorComponent* cursorComp, FMathDocumentState* meDocState);
	void Show();
	void Hide();
private:
	FTACursorComponent* m_cursorComp = nullptr;
	FMathDocumentState* m_meDocState = nullptr;
};