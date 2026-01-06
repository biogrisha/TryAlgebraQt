#include "CaretVisual.h"
#include "Modules/CursorComponent/CursorComponent.h"

CaretVisual::CaretVisual(FTACursorComponent* cursorComp, FMathDocumentState* meDocState)
{
	m_cursorComp = cursorComp;
	m_meDocState = meDocState;
}

void CaretVisual::Show()
{
	auto params = m_cursorComp->GetParameters();
	FCaretData caretData;
	caretData.Pos = glm::vec2{ params.Position.x - 2, params.Position.y };
	caretData.Size = glm::vec2{ 5, params.Height };
	m_meDocState->SetCaret(caretData);
}
