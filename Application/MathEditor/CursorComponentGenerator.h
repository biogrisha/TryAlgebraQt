#pragma once 
#include "MathDocumentState.h"
#include <Modules/MathElementsV2/CompatibilityData.h>
#include <Modules/CursorComponent/CursorComponent.h>
#include <Modules/Visual/VisualBase.h>

class CursorComponentGeneratorQt : public FTACursorComponentGenerator
{
public:
	virtual std::shared_ptr<FTACursorComponent> Create() override;
	FMathDocumentState* m_meDocState = nullptr;
};