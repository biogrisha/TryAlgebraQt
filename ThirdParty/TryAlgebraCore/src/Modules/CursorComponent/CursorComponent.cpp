// Fill out your copyright notice in the Description page of Project Settings.


#include "Modules/CursorComponent/CursorComponent.h"

#include "FunctionLibraries/MathElementsHelpers.h"
#include "Modules/MathElementsV2/Me/MeContainer.h"
#include "Modules/MathElementsV2/Me/MeDocument.h"
#include "Modules/MathElementsV2/Me/MeNewLine.h"
#include "Modules/Visual/VisualBase.h"

FTACursorComponent::FTACursorComponent()
{
	CurrentPath.TreePath.push_back(0);
	bStartedSelection = false;
}

void FTACursorComponent::SetVisual(const std::shared_ptr<class FTAVisual>& InVisual)
{
	Visual = InVisual;
}

void FTACursorComponent::Show()
{
	Redraw();
}

void FTACursorComponent::Hide()
{
	Visual->Hide();
}

FTACursorParameters FTACursorComponent::GetParameters()
{
	FTACursorParameters CursorParameters;
	//Find elements related to cursor
	TTypedWeak<MathElementV2::FTAMeComposite> ParentElement = FTAMeHelpers::GetParentElement(Document.Get(), CurrentPath);
	MathElementV2::FMathElements Children = ParentElement->GetChildren();
	//Find cursor height
	CursorParameters.Height = ParentElement->GetAccumulatedScalingFactor() * 30;
	
	auto LeafPath = CurrentPath.TreePath.back();
	if (LeafPath == 0)
	{
		if(Children.empty() || Children[0]->IsOfType(MathElementV2::FTAMeNewLine::StaticType()))
		{
			CursorParameters.Position = ParentElement->GetAbsolutePosition();
		}
		else
		{
			auto ChildPos = Children[0]->GetAbsolutePosition();
			auto HorizontalAlign = Children[0]->GetHorizontalAlignmentOffset();
			CursorParameters.Position.x = ChildPos.x;
			CursorParameters.Position.y = ChildPos.y + HorizontalAlign - CursorParameters.Height / 2.f;
		}
	}
	else
	{
		auto ChildPos = Children[LeafPath - 1]->GetAbsolutePosition();
		auto ChildSize = Children[LeafPath - 1]->GetAbsoluteSize();
		auto HorizontalAlign = Children[LeafPath - 1]->GetHorizontalAlignmentOffset();
		CursorParameters.Position.x = ChildPos.x + ChildSize.x;
		CursorParameters.Position.y = ChildPos.y + HorizontalAlign - CursorParameters.Height/2.f;
	}
	return CursorParameters;
}

FTAMePath FTACursorComponent::GetCurrentPath()
{
	return CurrentPath;
}

void FTACursorComponent::StepX(int Count)
{
	auto PathTemp = CurrentPath;
	FTAMeHelpers::MakeXStep(Document.Get(), PathTemp, Count);
	SetCurrentPath(PathTemp);
}

void FTACursorComponent::StepY(int Count)
{
	auto PathTemp = CurrentPath;
	FTAMeHelpers::MakeYStep(Document.Get(), PathTemp, Count);
	SetCurrentPath(PathTemp);
}

void FTACursorComponent::UpdateSelecting(const TACommonTypes::FTAVector2d& InPosition)
{
	if (!bStartedSelection)
	{
		bStartedSelection = true;
		FTAMeHelpers::GetIndexAtPosition(Document.Get(),InPosition,SelectionStart);
	}
	FTAMePath SelectionEndTemp;
	FTAMeHelpers::GetIndexAtPosition(Document.Get(),InPosition,SelectionEndTemp);
	Select(SelectionStart,SelectionEndTemp);
}

void FTACursorComponent::StartManualSelection()
{
	if (SelectedElements.empty())
	{
		SelectionStart = CurrentPath;
	}
}

void FTACursorComponent::UpdateManualSelection()
{
	Select(SelectionStart, CurrentPath, false);
}

void FTACursorComponent::Select(const FTAMePath& PathFrom, const FTAMePath& PathTo, bool bUpdateCurrentPath, bool bCallDelegate)
{
	SelectionStart = PathFrom;
	HighlightSelected(false);

	FTAMePath NewCurrentPath;
	SelectedElements = FTAMeHelpers::GetSelectedElements(Document.Get(), SelectionStart, PathTo, NewCurrentPath,bSelectedLeftToRight);
	SelectionEnd = NewCurrentPath;
	if (bUpdateCurrentPath)
	{
		SetCurrentPath(NewCurrentPath);
	}
	HighlightSelected(true);
	Visual->Show();
	
	if (bCallDelegate)
	{
		OnSelectionUpdated.Invoke(this);
	}
}

void FTACursorComponent::SelectAll()
{
	ClearSelection(false);
	if (Document->GetChildren().size() > 0)
	{
		FTAMePath PathFrom;
		PathFrom.TreePath.push_back(1);
		FTAMePath PathTo;
		PathTo.TreePath.push_back(Document->GetChildren().size());
		Select(PathFrom, PathTo);
	}
}

void FTACursorComponent::ClearSelection(bool bCallDelegate)
{
	StopSelecting();
	SelectionStart = CurrentPath;
	HighlightSelected(false);
	SelectedElements.clear();
	if (bCallDelegate)
	{
		OnSelectionUpdated.Invoke(this);
	}
}

void FTACursorComponent::StopSelecting()
{
	bStartedSelection = false;
}

FTAMePath FTACursorComponent::GetNormalizedPath()
{
	if (!SelectedElements.empty())
	{
		if (bSelectedLeftToRight)
		{
			FTAMePath Result = SelectionEnd;
			Result.TreePath.back() -= SelectedElements.size();
			return Result;
		}
		return SelectionEnd;
	}
	return CurrentPath;
}

void FTACursorComponent::SetCurrentPath(const FTAMePath& InPath)
{
	CurrentPath = InPath;
	HighlightParent();
}

void FTACursorComponent::OnMeAdded(const FTAMePath& Path, const MathElementV2::FMathElements& MathElements)
{
	//Set cursor after added elements
	FTAMePath NewCurrentPath = Path;
	NewCurrentPath.TreePath.back() += MathElements.size();
	SetCurrentPath(NewCurrentPath);
}

void FTACursorComponent::OnScrolled(float)
{
	Redraw();
}

void FTACursorComponent::OnMeRemoved(const FTAMePath& Path,const std::wstring&, int)
{
	bStartedSelection = false;
	SetCurrentPath(Path);
	SelectionStart = CurrentPath;
	SelectedElements.clear();
	Redraw();
}

void FTACursorComponent::OnRelativeHeightUpdated(float)
{
	Redraw();
}

void FTACursorComponent::OnComponentAdded()
{
	Document = Host->Cast<MathElementV2::FTAMeDocument>()->GetSelfWeak();
	OnMeAddedHandle = Document->OnMeAdded.AddFunc(this, &FTACursorComponent::OnMeAdded);
	OnScrolledHandle = Document->OnScrolled.AddFunc(this, &FTACursorComponent::OnScrolled);
	OnMeRemovedHandle = Document->OnMeRemoved.AddFunc(this, &FTACursorComponent::OnMeRemoved);
	OnRelativeHeightUpdatedHandle = Document->OnRelativeHeightUpdated.AddFunc(this, &FTACursorComponent::OnRelativeHeightUpdated);
}

bool FTACursorComponent::CursorInVisibleArea() const
{
	/*int IndexAdjusted = (std::max)(0,CurrentPath.TreePath[0]-1);
	if (IndexAdjusted < Document->GetVisibleTo() && IndexAdjusted >= Document->GetVisibleFrom())
	{
		return true;
	}
	return false;*/
	return true;
}

void FTACursorComponent::Redraw()
{
	if (CursorInVisibleArea())
	{
		//If elements are inside visible area->redraw cursor
		Visual->Show();
	}
	else
	{
		Visual->Hide();
	}
}

void FTACursorComponent::HighlightSelected(bool bVal)
{
	for (auto& Me : SelectedElements)
	{
		Me->bSelected = bVal;
	}
	FTAMeHelpers::RedrawIfVisible(SelectedElements);
}

void FTACursorComponent::HighlightParent()
{
	//Highlight parent element
	auto NewParent = FTAMeHelpers::GetParentElement(Document.Get(), CurrentPath);
	if (NewParent.IsValid() && NewParent != CurrentParent)
	{
		if (CurrentParent.IsValid() && !CurrentParent->IsOfType(MathElementV2::FTAMeDocument::StaticType()))
		{
			CurrentParent->bHighlight = false;
			FTAMeHelpers::RedrawIfVisible({CurrentParent});
		}
		CurrentParent = NewParent;
		if (!CurrentParent->IsOfType(MathElementV2::FTAMeDocument::StaticType()))
		{
			CurrentParent->bHighlight = true;
			FTAMeHelpers::RedrawIfVisible({CurrentParent});
		}
	}
}
