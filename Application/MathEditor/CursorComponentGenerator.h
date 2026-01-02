#pragma once 
#include "MathDocumentRenderingStructs.h"
#include "Modules/MathElementsV2/CompatibilityData.h"
#include "Modules/CursorComponent/CursorComponent.h"
#include "Modules/Visual/VisualBase.h"

class CursorComponentGeneratorQt : public FTACursorComponentGenerator
{
public:
	virtual std::shared_ptr<FTACursorComponent> Create() override;
	FCaretData* m_caretDataPtr = nullptr;
};