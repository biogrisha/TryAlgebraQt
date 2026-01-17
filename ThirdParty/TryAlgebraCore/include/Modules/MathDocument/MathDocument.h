// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include <string>
#include "Modules/ComponentBase.h"
#include "Modules/CommonTypes/Vector2D.h"
#include "Modules/MathElementsV2/MathElementPath.h"

namespace MathElementV2
{
	class FTAMeDocument;
}

class FTAMathDocument : public FTAComponentsHost
{
	TYPED_CLASS_META(FTAMathDocument, FTAComponentsHost)
public:
	void Setup(const std::shared_ptr<class FTACompatibilityData>& InCompatibilityData);
	void ClearDrawing();
	void Draw();
	void UpdateCaret();
	void SetMeData(const std::wstring& MathData);
	std::wstring GetMeData() const;
	TTypedWeak<MathElementV2::FTAMeDocument> GetMeDocument() const;
	void AddMathElements(const std::wstring& MathData);
	void AddSelectMathElements(const std::wstring& MathData);
	void StepX(int Count, bool bWithSelection = false);
	void StepY(int Count, bool bWithSelection = false);
	void ScrollY(int Count);
	void ScrollYDelta(float& Delta);
	void UpdateSelecting(const TACommonTypes::FTAVector2d& InPos);
	void SelectAll();
	void StopSelecting();
	void DeleteForward();
	void DeleteBackward();
	void CopySelected();
	void CutSelected();
	void Paste();
	void SetHeight(float Val);
	void Undo();
	void Redo();
	TTypedWeak<class FTACursorComponent> GetCursorComponent();
	const std::shared_ptr<class FTACompatibilityData>& GetCompatibilityData() const;
	std::wstring GetSelectedMeData() const;

private:
	void DeleteMeImpl(const FTAMePath& Path, int Count);
	std::shared_ptr<MathElementV2::FTAMeDocument> MeDocument;
	TTypedWeak<class FTACursorComponent> CursorComponent;
	TTypedWeak<class FTAActionHistoryComponent> ActionHistoryComponent;
	TACommonTypes::FTAVector2d CurrentSize = {100, 100};
	std::shared_ptr<class FTACompatibilityData> CompatibilityData;
	static inline std::wstring CopiedMathData;
};
