// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include <memory>
#include <vector>

#include "CursorComponent.h"
#include "Modules/ComponentBase.h"
#include "Modules/CommonTypes/MulticastDelegate.h"
#include "Modules/CommonTypes/Vector2D.h"
#include "Modules/MathElementsV2/Me/MathElementBaseV2.h"
#include "Modules/MathElementsV2/MathElementPath.h"

namespace MathElementV2
{
	class FTAMeDocument;
}
struct FTACursorParameters
{
	TACommonTypes::FTAVector2d Position;
	float Height;
};
class FTACursorComponent : public FTAComponentBase
{
	TYPED_CLASS_META(FTACursorComponent, FTAComponentBase);
protected:
	FTACursorComponent();
public:
	void SetVisual(const std::shared_ptr<class FTACursorVisualBase>& InVisual);
	void Show();
	void Hide();
	FTACursorParameters GetParameters();
	FTAMePath GetCurrentPath();
	void StepX(int Count);
	void StepY(int Count);
	void UpdateSelecting(const TACommonTypes::FTAVector2d& InPosition);
	void StartManualSelection();
	void UpdateManualSelection();
	//Use this with ClampPath()
	void Select(const FTAMePath& PathFrom, const FTAMePath& PathTo, bool bUpdateCurrentPath = true, bool bCallDelegate = true);
	void SelectAll();
	void ClearSelection(bool bCallDelegate = true);
	void StopSelecting();
	const std::vector<TTypedWeak<MathElementV2::FTAMathElementBase>>& GetSelectedElements(){return SelectedElements;}
	bool IsSelectedLeftToRight() const {return bSelectedLeftToRight;}
	//Path to element from the left of selection
	FTAMePath GetNormalizedPath();
	void SetCurrentPath(const FTAMePath& InPath);
protected:
	void OnMeAdded(const FTAMePath& Path, const MathElementV2::FMathElements& MathElements);
	void OnScrolled(float);
	void OnMeRemoved(const FTAMePath& Path,const std::wstring&, int);
	void OnRelativeHeightUpdated(float);
	virtual void OnComponentAdded() override;
	bool CursorInVisibleArea() const;
	void Redraw();
	void HighlightSelected(bool bVal);
	void HighlightParent();
public:
	FTAMulticastDelegate<FTACursorComponent*> OnSelectionUpdated;
protected:
	TTypedWeak<MathElementV2::FTAMeDocument> Document;
	std::shared_ptr<class FTACursorVisualBase> Visual;
	FTAMePath CurrentPath;
	FTAMePath SelectionStart;
	FTAMePath SelectionEnd;
	bool bStartedSelection;
	FTAMulticastDelegate<const FTAMePath&, const MathElementV2::FMathElements&>::HndlPtr OnMeAddedHandle;
	FTAMulticastDelegate<const FTAMePath&,const std::wstring&, int>::HndlPtr OnMeRemovedHandle;
	FTAMulticastDelegate<float>::HndlPtr OnScrolledHandle;
	FTAMulticastDelegate<float>::HndlPtr OnRelativeHeightUpdatedHandle;
	std::vector<TTypedWeak<MathElementV2::FTAMathElementBase>> SelectedElements;
	TTypedWeak<MathElementV2::FTAMeComposite> CurrentParent;
	bool bSelectedLeftToRight = true;
};
