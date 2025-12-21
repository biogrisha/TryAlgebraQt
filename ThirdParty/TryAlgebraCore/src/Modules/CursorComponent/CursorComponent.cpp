// Fill out your copyright notice in the Description page of Project Settings.


#include "Modules/CursorComponent/CursorComponent.h"

#include "FunctionLibraries/MathElementsHelpers.h"
#include "Modules/MathElementsV2/Me/MeContainer.h"
#include "Modules/MathElementsV2/Me/MeDocument.h"
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
	CursorParameters.Height = ParentElement->GetScalingFactor(0);
	if (CurrentPath.TreePath.back() == 0)
	{
		//if cursor at start
		if (Children.empty())
		{
			//Set cursor at parents local zero
			CursorParameters.Position = ParentElement->GetAbsolutePosition();
			if (ParentElement->IsOfType(MathElementV2::FTAMeContainer::StaticType()))
			{
				//if parent is container and no children->set cursor at center
				CursorParameters.Position.x += ParentElement->GetAbsoluteSize().x / 2.f;
			}
		}
		else
		{
			//If there are children at 0 index -> use its position and alignment
			CursorParameters.Position = Children[0]->GetAbsolutePosition() + TACommonTypes::FTAVector2d(0, Children[0]->GetHorizontalAlignmentOffset());
			CursorParameters.Position.y -= CursorParameters.Height / 2.f;
		}
	}
	else
	{
		//If the cursor is after some element
		//Find this element
		auto Me = Children[CurrentPath.TreePath.back() - 1];
		//Set the position according to the alignment
		auto ChildPos = Me->GetAbsolutePosition() + TACommonTypes::FTAVector2d(0, Me->GetHorizontalAlignmentOffset());
		ChildPos.y -= CursorParameters.Height / 2.f;
		ChildPos.x += Children[CurrentPath.TreePath.back() - 1]->GetAbsoluteSize().x;
		CursorParameters.Position = ChildPos;
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
	SetCurrentPath(ClampPath(PathTemp));
	Redraw();
}

void FTACursorComponent::StepY(int Count)
{
	auto PathTemp = CurrentPath;
	FTAMeHelpers::MakeYStep(Document.Get(), PathTemp, Count);
	SetCurrentPath(ClampPath(PathTemp));
	Redraw();
}

void FTACursorComponent::UpdateSelecting(const TACommonTypes::FTAVector2d& InPosition)
{
	if (!bStartedSelection)
	{
		bStartedSelection = true;
		FTAMeHelpers::GetIndexAtPosition(Document.Get(),InPosition,SelectionStart);
		ClampPath(SelectionStart);
	}
	FTAMePath SelectionEndTemp;
	FTAMeHelpers::GetIndexAtPosition(Document.Get(),InPosition,SelectionEndTemp);
	Select(SelectionStart, ClampPath(SelectionEndTemp));
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
	Redraw();
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
	int IndexAdjusted = (std::max)(0,CurrentPath.TreePath[0]-1);
	if (IndexAdjusted < Document->GetVisibleTo() && IndexAdjusted >= Document->GetVisibleFrom())
	{
		return true;
	}
	return false;
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

FTAMePath& FTACursorComponent::ClampPath(FTAMePath& Path) const
{
	Path.TreePath[0] = (std::max)(1,Path.TreePath[0]);
	return Path;
}
