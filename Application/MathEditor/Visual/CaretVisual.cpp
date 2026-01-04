#include "CaretVisual.h"
#include "Modules/CursorComponent/CursorComponent.h"

CaretVisual::CaretVisual(FTACursorComponent* cursorComp, FCaretData* caretDataPtr)
{
	m_cursorComp = cursorComp;
	m_caretDataPtr = caretDataPtr;
}

void CaretVisual::Show()
{
	auto params = m_cursorComp->GetParameters();
	m_caretDataPtr->Pos = glm::vec2{ params.Position.x - 2, params.Position.y };
	m_caretDataPtr->Size = glm::vec2{ 5, params.Height };
}
