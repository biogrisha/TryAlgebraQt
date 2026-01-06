// Fill out your copyright notice in the Description page of Project Settings.


#include "Modules/MathElementsV2/Me/MeDocument.h"

#include "FunctionLibraries/MathElementsHelpers.h"
#include "Modules/MathElementsV2/Me/MeContainer.h"
#include "Modules/MathElementsV2/Me/MeNewLine.h"
#include <algorithm>

MathElementV2::FTAMeDocument::FTAMeDocument()
{
	RelativeHeight = 1200.f;
	VisibleFrom = 0;
	VisibleTo = 0;
	LinesCount = 0;
	SetDefaultSize({1,1});
	//Init first line
	//auto FirstLine = FTAMeNewLine::MakeTypedShared();
	//FirstLine->SetParent(this);
	//Children.push_back(FirstLine);
}

float MathElementV2::FTAMeDocument::GetScalingFactor(int ChildIndex)
{
	return 1;
}

void MathElementV2::FTAMeDocument::ChildrenChanged(const FTAMePath& RequestPath, bool bSizeChanged)
{
	bool bOutsideVisibleArea = RequestPath.TreePath.back() > VisibleTo || (std::max)(0, RequestPath.TreePath.back() - 1) < VisibleFrom;
	if (!bSizeChanged && !bOutsideVisibleArea)
	{
		//If the size hasn't changed, simply redraw the modified element.
		FTAMeHelpers::ShowElements(Children, RequestPath.TreePath.back(), RequestPath.TreePath.back() + 1);
		return;
	}
	//If size changed
	std::vector<TTypedWeak<FTAMathElementBase>> VisibleElementsToRedraw;
	//Find line start/end
	int AffectedLineStart;
	FTAMeNewLine* MeNewLine = FTAMeHelpers::FindLineStart(this, RequestPath.TreePath.back(), AffectedLineStart);
	int AffectedLineEnd = AffectedLineStart + MeNewLine->GetElementsCount();
	if (bOutsideVisibleArea)
	{
		//If Path outside visible region
		//Cache elements that can be affected by insertion
		VisibleElementsToRedraw = FTAMeHelpers::GetMeRange(Children, AffectedLineStart, VisibleTo);
		FTAMeHelpers::SetShouldHide(VisibleElementsToRedraw);
	}

	//Arrange math elements and find last visible index
	FTAMeHelpers::CalculateMeCountInLines(this, AffectedLineStart, AffectedLineEnd);
	PreAligned.Invoke(Children, AffectedLineStart, AffectedLineEnd);
	FTAMeHelpers::ArrangeElementsInLines(this, AffectedLineStart, AffectedLineEnd);
	if (!bOutsideVisibleArea)
	{
		//If elements are inside visible area
		if (AffectedLineStart == VisibleFrom)
		{
			//If this is first line in visible area-> we don't want to take line above into account
			FTAMeHelpers::ArrangeLines(this, 0, AffectedLineStart, AffectedLineEnd);
		}
		else
		{
			FTAMeHelpers::ArrangeLines(this, AffectedLineStart, AffectedLineEnd);
		}
		//Update visible to nd redraw needed elements
		VisibleTo = AffectedLineEnd;
		FTAMeHelpers::ShowElements(Children, AffectedLineStart, AffectedLineEnd);
		FTAMeHelpers::HideIfShould(VisibleElementsToRedraw);
	}
}

void MathElementV2::FTAMeDocument::AddMathElements(const FTAMePath& Path, const std::wstring& InMathElements)
{
	//Generate math elements from string
	FMathElements GeneratedMathElements = FTAMeHelpers::ParseFromString(InMathElements, Generator.lock().get());
	if (GeneratedMathElements.empty())
	{
		return;
	}

	TTypedWeak<FTAMeComposite> ParentElement = FTAMeHelpers::GetParentElement(this, Path);
	//set this as a parent
	for (auto& Me : GeneratedMathElements)
	{
		//Set parent
		Me->SetParent(ParentElement.Get());
	}
	
	ParentElement->AddChildren(Path, GeneratedMathElements);
	if(ParentElement.Get() != this)
	{
		Children[Path.TreePath[0]]->CalculateSize(Children[Path.TreePath[0]]->GetParent()->GetAccumulatedScalingFactor());
	}
	else
	{
		FTAMeHelpers::CalculateSize(GeneratedMathElements);
	}
	ArrangeVisibleElements();
	OnMeAdded.Invoke(Path, GeneratedMathElements);
}

void MathElementV2::FTAMeDocument::RemoveMathElements(FTAMePath Path, int Count)
{
	TTypedWeak<FTAMeComposite> ParentElement = FTAMeHelpers::GetParentElement(this, Path);

	auto CachedMe = ParentElement->RemoveChildren(Path, Count);
	if (ParentElement.Get() != this)
	{
		Children[Path.TreePath[0]]->CalculateSize(Children[Path.TreePath[0]]->GetParent()->GetAccumulatedScalingFactor());
	}
	ArrangeVisibleElements();
	OnMeRemoved.Invoke(Path, CachedMe, Count);
}

void MathElementV2::FTAMeDocument::SetMeGenerator(const std::weak_ptr<FTAMathElementGenerator>& InGenerator)
{
	Generator = InGenerator;
}

void MathElementV2::FTAMeDocument::ScrollY(int Count)
{
	int VisibleFromOld = VisibleFrom;
	int VisibleToOld = VisibleTo;
	VisibleFrom = FTAMeHelpers::ScrollY(this, Count);
	//Set line pose to zero
	//Arrange lines while elements are within screen area
	FTAMeHelpers::ArrangeLines(this, 0, VisibleFrom, VisibleTo);
	if (Count > 0)
	{
		//Hide elements above
		FTAMeHelpers::HideElements(Children, VisibleFromOld, VisibleFrom);
	}
	else
	{
		//Hide elements below
		FTAMeHelpers::HideElements(Children, VisibleTo, VisibleToOld);
	}
	FTAMeHelpers::ShowElements(Children, VisibleFrom, VisibleTo);
	InvokeOnScrolled();
}

void MathElementV2::FTAMeDocument::ScrollYDelta(float& Delta)
{
	if (Children.empty())
	{
		Delta = 0;
		return;
	}
	int NewVisibleFrom = Children.size() * Delta;
	FTAMeHelpers::FindLineStart(this, NewVisibleFrom, NewVisibleFrom);
	Delta = (float)NewVisibleFrom / (float)Children.size();
	if (NewVisibleFrom == VisibleFrom)
	{
		return;
	}
	//Hide elements
	FTAMeHelpers::HideElements(Children, VisibleFrom, VisibleTo);
	VisibleFrom = NewVisibleFrom;
	//Arrange lines while elements are within screen area
	FTAMeHelpers::ArrangeLines(this, 0, VisibleFrom, VisibleTo);
	FTAMeHelpers::ShowElements(Children, VisibleFrom, VisibleTo);
	InvokeOnScrolled();
}

void MathElementV2::FTAMeDocument::InvokeOnScrolled()
{
	float ScrollDelta = 0;
	if (Children.size() > 0)
	{
		ScrollDelta = float(VisibleFrom)/float(Children.size());
	}
	OnScrolled.Invoke(ScrollDelta);
}

void MathElementV2::FTAMeDocument::SetRelativeHeight(float InRelativeHeight)
{
	std::vector<TTypedWeak<FTAMathElementBase>> VisibleElementsToRedraw;
	if (RelativeHeight > InRelativeHeight)
	{
		VisibleElementsToRedraw = FTAMeHelpers::GetMeRange(Children, VisibleFrom, VisibleTo);
		FTAMeHelpers::SetShouldHide(VisibleElementsToRedraw);
	}
	RelativeHeight = InRelativeHeight;
	FTAMeHelpers::ArrangeLines(this, 0, VisibleFrom, VisibleTo);
	FTAMeHelpers::ShowElements(Children, VisibleFrom, VisibleTo);
	FTAMeHelpers::HideIfShould(VisibleElementsToRedraw);
	OnRelativeHeightUpdated.Invoke(RelativeHeight);
}

void MathElementV2::FTAMeDocument::UpdateLinesCount(int Count)
{
	LinesCount += Count;
	OnLinesCountUpdated.Invoke(LinesCount);
}

void MathElementV2::FTAMeDocument::SetDocumentToChildren(const FMathElements& MathElements)
{
	for (auto& Me : MathElements)
	{
		if (auto Comp = Me->Cast<FTAMeComposite>())
		{
			if (auto Cont = Me->Cast<FTAMeContainer>())
			{
				Cont->SetOwnerDocument(GetSelfWeak());
			}
			SetDocumentToChildren(Comp->GetChildren());
		}
	}
}

void MathElementV2::FTAMeDocument::ArrangeVisibleElements()
{

	if (Children.empty())
	{
		VisibleFrom = 0;
		VisibleTo = 0;
	}
	float MaxHorizontalAlignment = FLT_MIN;
	float YOffset = 0;
	float XOffset = 0;
	int LineStart = VisibleFrom;
	for (int i = VisibleFrom; i < Children.size(); i++)
	{
		auto NewLine = Children[i]->Cast<MathElementV2::FTAMeNewLine>();
		if (NewLine)
		{
			//if new line
			//lower all elements in line by max hor alignment and YOffset
			float MaxYOffset = NewLine->GetAbsoluteSize().y;
			for (int j = LineStart; j < i; j++)
			{
				auto LocalPos = Children[j]->GetLocalPosition();
				LocalPos.y += MaxHorizontalAlignment + YOffset;
				MaxYOffset = std::max(MaxYOffset, LocalPos.y + Children[j]->GetAbsoluteSize().y);
				Children[j]->SetLocalPosition(LocalPos);
			}
			LineStart = i;
			MaxHorizontalAlignment = FLT_MIN;
			XOffset = 0;
			YOffset = MaxYOffset;
			VisibleTo = LineStart;
			if (YOffset >= RelativeHeight)
			{
				return;
			}
		}

		TACommonTypes::FTAVector2d NewPos;
		auto HorAlign = Children[i]->GetHorizontalAlignmentOffset();
		//find max hor alignment
		MaxHorizontalAlignment = std::max(MaxHorizontalAlignment, HorAlign);
		//move element up by hor alignment
		NewPos.y = -HorAlign;
		NewPos.x = XOffset;
		Children[i]->SetLocalPosition(NewPos);
		//Advance X
		XOffset += Children[i]->GetAbsoluteSize().x;

		if (i + 1 == Children.size())
		{
			VisibleTo = Children.size();
			for (int j = LineStart; j <= i; j++)
			{
				auto LocalPos = Children[j]->GetLocalPosition();
				LocalPos.y += MaxHorizontalAlignment + YOffset;
				Children[j]->SetLocalPosition(LocalPos);
			}
		}
	}
}
