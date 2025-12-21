// Fill out your copyright notice in the Description page of Project Settings.


#include "Modules/ActionHistory/ActionHistoryComponent.h"

#include "Modules/ActionHistory/ActionAddMe.h"
#include "Modules/ActionHistory/ActionRemoveMe.h"
#include "Modules/ActionHistory/ActionReplaceMe.h"
#include "Modules/MathElementsV2/Me/MeDocument.h"


void FTAActionHistoryComponent::OnComponentAdded()
{
	MeDocument = Host->Cast<MathElementV2::FTAMeDocument>()->GetSelfWeak();
}

void FTAActionHistoryComponent::RecordAddMe(bool bEnable, bool bUndo, bool bClearRedo)
{
	if (bEnable)
	{
		ListenMeAdded(true);
		bMeAdded = false;
	}
	else
	{
		ListenMeAdded(false);
		if (bMeAdded)
		{
			std::shared_ptr<FTAActionBase> Action = FTAActionAddMe::MakeTypedShared(MeDocument, Path, Count);
			bUndo ? AddUndoAction(Action) : AddRedoAction(Action);
			if (bClearRedo && bUndo)
			{
				RedoActions.clear();
			}
		}
	}
}

void FTAActionHistoryComponent::RecordRemoveMe(bool bEnable, bool bUndo,bool bClearRedo)
{
	if (bEnable)
	{
		ListenMeRemoved(true);
		bMeRemoved = false;
	}
	else
	{
		ListenMeRemoved(false);
		if (bMeRemoved)
		{
			std::shared_ptr<FTAActionBase> Action = FTAActionRemoveMe::MakeTypedShared(MeDocument,Path,MeData);
			bUndo ? AddUndoAction(Action) : AddRedoAction(Action);
			if (bClearRedo && bUndo)
			{
				RedoActions.clear();
			}
		}
	}
}

void FTAActionHistoryComponent::RecordReplaceMe(bool bEnable, bool bUndo,bool bClearRedo)
{
	if (bEnable)
	{
		ListenMeAdded(true);
		ListenMeRemoved(true);
		bMeAdded = false;
		bMeRemoved = false;
	}
	else
	{
		ListenMeAdded(false);
		ListenMeRemoved(false);
		if (bMeAdded && bMeRemoved)
		{
			std::shared_ptr<FTAActionBase> Action = FTAActionReplaceMe::MakeTypedShared(MeDocument,Path,MeData, Count);
			bUndo ? AddUndoAction(Action) : AddRedoAction(Action);
			if (bClearRedo && bUndo)
			{
				RedoActions.clear();
			}
		}
	}
}

void FTAActionHistoryComponent::ReverseAction(bool bUndo)
{
	if (bUndo ? UndoActions.empty() : RedoActions.empty())
	{
		return;
	}
	auto& Action = bUndo ? UndoActions.back() : RedoActions.back();
	if (Action->GetType() == FTAActionAddMe::StaticType())
	{
		RecordRemoveMe(true);
		Action->Undo();
		RecordRemoveMe(false, !bUndo,false);
	}
	else if (Action->GetType() == FTAActionRemoveMe::StaticType())
	{
		RecordAddMe(true);
		Action->Undo();
		RecordAddMe(false, !bUndo,false);
	}
	else if (Action->GetType() == FTAActionReplaceMe::StaticType())
	{
		RecordReplaceMe(true);
		Action->Undo();
		RecordReplaceMe(false, !bUndo,false);
	}
	bUndo ? UndoActions.pop_back() : RedoActions.pop_back();
}

void FTAActionHistoryComponent::ListenMeAdded(bool bEnable)
{
	if (bEnable)
	{
		OnMeAddedHandle = MeDocument->OnMeAdded.AddFunc(this, &FTAActionHistoryComponent::OnMeAdded);
	}
	else
	{
		OnMeAddedHandle.reset();
	}
}

void FTAActionHistoryComponent::ListenMeRemoved(bool bEnable)
{
	if (bEnable)
	{
		OnMeRemovedHandle = MeDocument->OnMeRemoved.AddFunc(this, &FTAActionHistoryComponent::OnMeRemoved);
	}
	else
	{
		OnMeRemovedHandle.reset();
	}
}

void FTAActionHistoryComponent::OnMeAdded(const FTAMePath& InPath, const MathElementV2::FMathElements& MathElements)
{
	Path = InPath;
	Count = MathElements.size();
	bMeAdded = true;
}

void FTAActionHistoryComponent::OnMeRemoved(const FTAMePath& InPath, const std::wstring& InMeData, int InCount)
{
	Path = InPath;
	MeData = InMeData;
	bMeRemoved = true;
}

void FTAActionHistoryComponent::AddUndoAction(std::shared_ptr<class FTAActionBase>& Action)
{
	UndoActions.push_back(Action);
	if (UndoActions.size() > 50)
	{
		UndoActions.erase(UndoActions.begin(), UndoActions.begin() + 10);
	}
}

void FTAActionHistoryComponent::AddRedoAction(std::shared_ptr<class FTAActionBase>& Action)
{
	RedoActions.push_back(Action);
	if (RedoActions.size() > 50)
	{
		RedoActions.erase(RedoActions.begin(), RedoActions.begin() + 10);
	}
}
