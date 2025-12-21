#pragma once
#include "Modules/ComponentBase.h"
#include "Modules/CommonTypes/MulticastDelegate.h"
#include "Modules/MathElementsV2/MathElementPath.h"
#include "Modules/MathElementsV2/Me/MathElementBaseV2.h"

class FTACursorComponent;
class FTAMathDocument;

class FTAHighlightSameAsSelected : public FTAComponentBase
{
	TYPED_CLASS_META(FTAHighlightSameAsSelected,FTAComponentBase);
public:
	virtual void OnComponentAdded() override;

private:
	void OnSelectionUpdated(FTACursorComponent* InCursorComponent);
	void OnMeRemoved(const FTAMePath& MePath, const std::wstring& MeData, int Index);
	void HighlightMe(bool bVal);
	TTypedWeak<FTAMathDocument> Document;
	FTAMulticastDelegate<FTACursorComponent*>::HndlPtr SelectionUpdatedHndl;
	FTAMulticastDelegate<const FTAMePath&, const std::wstring&, int>::HndlPtr MeRemovedHndl;
	std::vector<MathElementV2::FMathElements> FoundMe;
};
