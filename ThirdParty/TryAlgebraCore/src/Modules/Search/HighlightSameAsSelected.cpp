#include "Modules/Search/HighlightSameAsSelected.h"

#include "FunctionLibraries/MathElementsHelpers.h"
#include "Modules/CursorComponent/CursorComponent.h"
#include "Modules/MathDocument/MathDocument.h"
#include "Modules/MathElementsV2/Me/MeDocument.h"


void FTAHighlightSameAsSelected::OnComponentAdded()
{
	Document = Host->Cast<FTAMathDocument>()->GetSelfWeak();
	SelectionUpdatedHndl = Document->GetCursorComponent()->OnSelectionUpdated
	                               .AddFunc(this, &FTAHighlightSameAsSelected::OnSelectionUpdated);
	MeRemovedHndl = Document->GetMeDocument()->OnMeRemoved.AddFunc(this, &FTAHighlightSameAsSelected::OnMeRemoved);
}

void FTAHighlightSameAsSelected::OnSelectionUpdated(FTACursorComponent* InCursorComponent)
{
	HighlightMe(false);
	FoundMe.clear();
	auto& SelectedMe = InCursorComponent->GetSelectedElements();
	if (SelectedMe.size() < 10)
	{
		auto MeDocument = Document->GetMeDocument();
		FTAMeHelpers::FindMe(SelectedMe, MeDocument->GetChildren(),MeDocument->GetVisibleFrom(),MeDocument->GetVisibleTo(),FoundMe);
		HighlightMe(true);
	}
}

void FTAHighlightSameAsSelected::OnMeRemoved(const FTAMePath& MePath, const std::wstring& MeData, int Index)
{
	HighlightMe(false);
	FoundMe.clear();
}

void FTAHighlightSameAsSelected::HighlightMe(bool bVal)
{
	for (auto& MathElements : FoundMe)
	{
		for (auto& Me : MathElements)
		{
			Me->bSelectionMatchHighlight = bVal;
		}
		FTAMeHelpers::RedrawIfVisible(MathElements);
	}
}
