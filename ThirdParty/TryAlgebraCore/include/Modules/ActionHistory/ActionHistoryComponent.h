// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Modules/ComponentBase.h"
#include "Modules/CommonTypes/MulticastDelegate.h"
#include "Modules/MathElementsV2/MathElementPath.h"
#include "Modules/MathElementsV2/Me/MathElementBaseV2.h"

namespace MathElementV2
{
	class FTAMeDocument;
}

class FTAActionHistoryComponent : public FTAComponentBase
{
	TYPED_CLASS_META(FTAActionHistoryComponent, FTAComponentBase);
protected:
	virtual void OnComponentAdded() override;
public:
	void RecordAddMe(bool bEnable, bool bUndo = true, bool bClearRedo = true);
	void RecordRemoveMe(bool bEnable, bool bUndo = true, bool bClearRedo = true);
	void RecordReplaceMe(bool bEnable, bool bUndo = true, bool bClearRedo = true);
	void ReverseAction(bool bUndo);
private:
	void ListenMeAdded(bool bEnable);
	void ListenMeRemoved(bool bEnable);
	void OnMeAdded(const FTAMePath& InPath, const MathElementV2::FMathElements& MathElements);
	void OnMeRemoved(const FTAMePath& InPath, const std::wstring& InMeData, int InCount);

	void AddUndoAction(std::shared_ptr<class FTAActionBase>& Action);
	void AddRedoAction(std::shared_ptr<class FTAActionBase>& Action);
	FTAMulticastDelegate<const FTAMePath&, const MathElementV2::FMathElements&>::HndlPtr OnMeAddedHandle;
	FTAMulticastDelegate<const FTAMePath&, const std::wstring&, int>::HndlPtr OnMeRemovedHandle;
	TTypedWeak<MathElementV2::FTAMeDocument> MeDocument;
	std::vector<std::shared_ptr<class FTAActionBase>> UndoActions;
	std::vector<std::shared_ptr<class FTAActionBase>> RedoActions;
	int Count = 0;
	std::wstring MeData;
	FTAMePath Path;
	bool bMeAdded = false;
	bool bMeRemoved = false;
};
