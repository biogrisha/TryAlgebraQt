#include "CaretVisual.h"
#include <Modules/CursorComponent/CursorComponent.h>

CaretVisual::CaretVisual(FTACursorComponent* cursorComp, FMathDocumentState* meDocState)
{
	m_cursorComp = cursorComp;
	m_meDocState = meDocState;
}

void CaretVisual::Show()
{
	//collecting caret visual parameters
	auto params = m_cursorComp->GetParameters();
	FCaretData caretData;
	//moving it slightly to the left
	caretData.Pos = glm::vec2{ params.Position.x - 2, params.Position.y };
	caretData.Size = glm::vec2{ 5, params.Height };
	//Updating caret info
	m_meDocState->SetCaret(caretData);
}

void CaretVisual::Hide()
{
	FCaretData caretData;
	//moving it slightly to the left
	caretData.Pos = glm::vec2{ -100, -100 };
	caretData.Size = glm::vec2{ 1, 1 };
	//Updating caret info
	m_meDocState->SetCaret(caretData);
}
