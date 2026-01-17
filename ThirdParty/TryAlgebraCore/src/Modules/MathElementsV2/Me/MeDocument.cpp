// Fill out your copyright notice in the Description page of Project Settings.


#include "Modules/MathElementsV2/Me/MeDocument.h"
#include <algorithm>

#include "FunctionLibraries/MathElementsHelpers.h"
#include "Modules/MathElementsV2/Me/MeContainer.h"
#include "Modules/MathElementsV2/Me/MeNewLine.h"
MathElementV2::FTAMeDocument::FTAMeDocument()
{
	Height = 1200.f;
	VisibleFrom = 0;
	VisibleTo = 0;
	LinesCount = 0;
	FirstLineMeCount = 0;
	CurrentLine = 0;
	LinesOnPage = 20;
	SetDefaultSize({1,1});
}

float MathElementV2::FTAMeDocument::GetScalingFactor(int ChildIndex)
{
	return 1;
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
		UpdateLinesCount(FTAMeHelpers::GetLinesCount(GeneratedMathElements));
		FTAMeHelpers::CalculateMeCountInLines(this, Path.TreePath[0], Path.TreePath[0] + GeneratedMathElements.size());
		FTAMeHelpers::CalculateSize(GeneratedMathElements);
	}
	ArrangeVisibleElements();
	OnMeAdded.Invoke(Path, GeneratedMathElements);
}

void MathElementV2::FTAMeDocument::RemoveMathElements(FTAMePath Path, int Count)
{
	TTypedWeak<FTAMeComposite> ParentElement = FTAMeHelpers::GetParentElement(this, Path);
	if (ParentElement.Get() == this)
	{
		UpdateLinesCount(-FTAMeHelpers::GetLinesCount(Children, Path.TreePath[0], Path.TreePath[0] + Count));
	}
	auto CachedMe = ParentElement->RemoveChildren(Path, Count);
	if (ParentElement.Get() != this)
	{
		Children[Path.TreePath[0]]->CalculateSize(Children[Path.TreePath[0]]->GetParent()->GetAccumulatedScalingFactor());
	}
	else
	{
		FTAMeHelpers::CalculateMeCountInLines(this, Path.TreePath[0]-1, Path.TreePath[0]);
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
	
}

void MathElementV2::FTAMeDocument::ScrollYDelta(float& Delta)
{

}

void MathElementV2::FTAMeDocument::InvokeOnScrolled()
{
	
}

void MathElementV2::FTAMeDocument::SetHeight(float InHeight)
{
	Height = InHeight;
	ArrangeVisibleElements();
}

void MathElementV2::FTAMeDocument::UpdateLinesCount(int Count)
{
	LinesCount += Count;
	OnLinesCountUpdated.Invoke(LinesCount, LinesOnPage);
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
			if (YOffset >= Height)
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
