// Fill out your copyright notice in the Description page of Project Settings.


#include "Modules/MathDocument/MathDocument.h"

#include "FunctionLibraries/MathElementsHelpers.h"
#include "Modules/ActionHistory/ActionHistoryComponent.h"
#include "Modules/CursorComponent/CursorComponent.h"
#include "Modules/MathElementsV2/BracketsAdjustComponent.h"
#include "Modules/MathElementsV2/MeIndexAdjustComponent.h"
#include "Modules/MathElementsV2/Me/MeDocument.h"

void FTAMathDocument::Setup(const std::shared_ptr<class FTACompatibilityData>& InCompatibilityData)
{
	MeDocument = MathElementV2::FTAMeDocument::MakeTypedShared();
	CompatibilityData = InCompatibilityData;
	MeDocument->SetMeGenerator(InCompatibilityData->MeGenerator);
	//Add cursor comp
	auto CursorCompPtr = InCompatibilityData->CursorComponentGenerator->Create();
	CursorComponent = CursorCompPtr->GetSelfWeak();
	MeDocument->AddComponent(CursorCompPtr);
	//Add history comp
	auto ActionHistoryCompPtr = FTAActionHistoryComponent::MakeTypedShared();
	ActionHistoryComponent = ActionHistoryCompPtr->GetSelfWeak();
	MeDocument->AddComponent(ActionHistoryCompPtr);
	//Add Brackets adjust comp
	auto BracketsAdjustComp = FTABracketsAdjustComponent::MakeTypedShared();
	MeDocument->AddComponent(BracketsAdjustComp);
	//Add Index adjust comp
	auto IndexAdjustComp = FTAMeIndexAdjustComponent::MakeTypedShared();
	MeDocument->AddComponent(IndexAdjustComp);
}

void FTAMathDocument::ClearDrawing()
{
	FTAMeHelpers::HideElements({MeDocument});
	CursorComponent->Hide();
}

void FTAMathDocument::Draw()
{
	FTAMeHelpers::ShowElements(MeDocument->GetChildren(), MeDocument->GetVisibleFrom(), MeDocument->GetVisibleTo());
}

void FTAMathDocument::UpdateCaret()
{
	CursorComponent->Show();
}

void FTAMathDocument::SetMeData(const std::wstring& MathData)
{
	FTAMePath Path;
	Path.TreePath.push_back(0);
	MeDocument->AddMathElements(Path, MathData);
}

std::wstring FTAMathDocument::GetMeData() const
{
	return FTAMeHelpers::ParseToString(MeDocument->GetChildren(), 1, MeDocument->GetChildren().size());
}

TTypedWeak<MathElementV2::FTAMeDocument> FTAMathDocument::GetMeDocument() const
{
	return MeDocument->GetSelfWeak();
}

void FTAMathDocument::AddMathElements(const std::wstring& MathData)
{
	std::vector<TTypedWeak<MathElementV2::FTAMathElementBase>> SelectedMe = CursorComponent->GetSelectedElements();
	if (!SelectedMe.empty())
	{
		//If there are selected elements -> replace them
		ActionHistoryComponent->RecordReplaceMe(true);
		FTAMePath Path = CursorComponent->GetNormalizedPath();
		MeDocument->RemoveMathElements(Path, SelectedMe.size());
		MeDocument->AddMathElements(Path, MathData);
		ActionHistoryComponent->RecordReplaceMe(false);
		return;
	}
	//If no me selected -> simply add me
	ActionHistoryComponent->RecordAddMe(true,true);
	FTAMePath Path = CursorComponent->GetCurrentPath();
	MeDocument->AddMathElements(Path,MathData);
	ActionHistoryComponent->RecordAddMe(false,true);
}

void FTAMathDocument::AddSelectMathElements(const std::wstring& MathData)
{
	FTAMePath Path = CursorComponent->GetNormalizedPath();
	AddMathElements(MathData);
	CursorComponent->Select(Path, CursorComponent->GetCurrentPath());
}

void FTAMathDocument::StepX(int Count, bool bWithSelection)
{
	if (bWithSelection)
	{
		CursorComponent->StartManualSelection();
		CursorComponent->StepX(Count);
		CursorComponent->UpdateManualSelection();
		return;
	}
	CursorComponent->ClearSelection();
	CursorComponent->StepX(Count);
}

void FTAMathDocument::StepY(int Count, bool bWithSelection)
{
	if (bWithSelection)
	{
		CursorComponent->StartManualSelection();
		CursorComponent->StepY(Count);
		CursorComponent->UpdateManualSelection();
		return;
	}
	CursorComponent->ClearSelection();
	CursorComponent->StepY(Count);
}

void FTAMathDocument::ScrollY(int Count)
{
	MeDocument->ScrollY(Count);
}

void FTAMathDocument::ScrollYDelta(float& Delta)
{
	MeDocument->ScrollYDelta(Delta);
}

void FTAMathDocument::UpdateSelecting(const TACommonTypes::FTAVector2d& InPos)
{
	CursorComponent->UpdateSelecting(InPos);
}

void FTAMathDocument::SelectAll()
{
	CursorComponent->SelectAll();
}

void FTAMathDocument::StopSelecting()
{
	CursorComponent->StopSelecting();
}

void FTAMathDocument::DeleteForward()
{
	FTAMePath CurrentPath = CursorComponent->GetNormalizedPath();
	if (CurrentPath.TreePath.size() == 1 && CurrentPath.TreePath.back() == MeDocument->GetChildren().size())
	{
		//if cursor at the end and nothing is selected
		return;
	}
	int Count = (std::max)((int)CursorComponent->GetSelectedElements().size(),1);
	if (CurrentPath.TreePath.size() == 1)
	{
		//If remove from MeDocument
		DeleteMeImpl(CurrentPath, Count);
		return;
	}
	if (auto ParentElement = FTAMeHelpers::GetParentElement(MeDocument.get(), CurrentPath).Get())
	{
		if (CurrentPath.TreePath.back() + Count <= ParentElement->GetChildren().size())
		{
			DeleteMeImpl(CurrentPath, Count);
		}
		else
		{
			CurrentPath.TreePath.erase(CurrentPath.TreePath.end()-2,CurrentPath.TreePath.end());
			DeleteMeImpl(CurrentPath, 1);
		}	
	}
}

void FTAMathDocument::DeleteBackward()
{
	FTAMePath CurrentPath = CursorComponent->GetNormalizedPath();
	
	int Count = CursorComponent->GetSelectedElements().size();
	if (Count == 0)
	{
		if (CurrentPath.TreePath.size() == 1 && CurrentPath.TreePath.back() == 0)
		{
			return;
		}
		if (CurrentPath.TreePath.back() == 0)
		{
			//If in container -> remove parent
			CurrentPath.TreePath.erase(CurrentPath.TreePath.end() - 2, CurrentPath.TreePath.end());
			DeleteMeImpl(CurrentPath, 1);
			return;
		}
		//If not at 0 -> step back and remove one element
		CurrentPath.TreePath.back()--;
		DeleteMeImpl(CurrentPath, 1);
		return;
	}
	//Remove selected elements
	DeleteMeImpl(CurrentPath, Count);
}

void FTAMathDocument::CopySelected()
{
	std::vector<TTypedWeak<MathElementV2::FTAMathElementBase>> SelectedMe = CursorComponent->GetSelectedElements();
	CopiedMathData = FTAMeHelpers::ParseToString(SelectedMe);
}

void FTAMathDocument::CutSelected()
{
	CopySelected();
	DeleteBackward();
}

void FTAMathDocument::Paste()
{
	AddMathElements(CopiedMathData);
}

void FTAMathDocument::SetHeight(float Val)
{
	MeDocument->SetHeight(Val);
}

void FTAMathDocument::Undo()
{
	CursorComponent->ClearSelection();
	ActionHistoryComponent->ReverseAction(true);
}

void FTAMathDocument::Redo()
{
	CursorComponent->ClearSelection();
	ActionHistoryComponent->ReverseAction(false);
}

TTypedWeak<class FTACursorComponent> FTAMathDocument::GetCursorComponent()
{
	return CursorComponent;
}

const std::shared_ptr<class FTACompatibilityData>& FTAMathDocument::GetCompatibilityData() const
{
	return CompatibilityData;
}

std::wstring FTAMathDocument::GetSelectedMeData() const
{
	std::vector<TTypedWeak<MathElementV2::FTAMathElementBase>> SelectedMe = CursorComponent->GetSelectedElements();
	return FTAMeHelpers::ParseToString(SelectedMe);
}

void FTAMathDocument::DeleteMeImpl(const FTAMePath& Path, int Count)
{
	ActionHistoryComponent->RecordRemoveMe(true,true);
	MeDocument->RemoveMathElements(Path, Count);
	ActionHistoryComponent->RecordRemoveMe(false,true);
}

