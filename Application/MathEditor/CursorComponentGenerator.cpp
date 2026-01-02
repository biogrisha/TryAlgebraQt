#include "CursorComponentGenerator.h"
#include "Visual/CaretVisual.h"
std::shared_ptr<FTACursorComponent> CursorComponentGeneratorQt::Create()
{
	auto cursorComponent = FTACursorComponent::MakeTypedShared();
	cursorComponent->SetVisual(std::make_shared<CaretVisual>(cursorComponent.get(), m_caretDataPtr));
	return cursorComponent;
}
