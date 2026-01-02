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
	m_caretDataPtr->Pos = glm::vec2{ params.Position.x, params.Position.y - 42/2 };
	m_caretDataPtr->Size = glm::vec2{ 5, params.Height * 42 };
}
