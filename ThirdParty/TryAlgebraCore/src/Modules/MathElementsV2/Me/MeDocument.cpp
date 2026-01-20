// Fill out your copyright notice in the Description page of Project Settings.


#include "Modules/MathElementsV2/Me/MeDocument.h"
#include <algorithm>

#include "FunctionLibraries/MathElementsHelpers.h"
#include "Modules/MathElementsV2/Me/MeContainer.h"
#include "Modules/MathElementsV2/Me/MeNewLine.h"
MathElementV2::FTAMeDocument::FTAMeDocument()
{
	Height = 1200.f;
	VisibleFrom = -1;
	VisibleTo = 0;
	LinesCount = 1;
	FirstLineMeCount = 0;
	CurrentLine = 0;
	bTextFillsVisibleArea = false;
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
		//No elements were generated
		return;
	}

	//finding parent element
	TTypedWeak<FTAMeComposite> ParentElement = FTAMeHelpers::GetParentElement(this, Path);
	//setting parent elment to generated elements
	for (auto& Me : GeneratedMathElements)
	{
		Me->SetParent(ParentElement.Get());
	}
	
	//insert math elements into parent
	ParentElement->AddChildren(Path, GeneratedMathElements);
	bool bAddedToDoc = ParentElement.Get() == this;
	if(!bAddedToDoc)
	{
		//Elements were added to child element
		//Recalculating the size of that child element
		Children[Path.TreePath[0]]->CalculateSize(Children[Path.TreePath[0]]->GetParent()->GetAccumulatedScalingFactor());
	}
	else
	{
		//Elements were added directly into parent
		FTAMeHelpers::CalculateSize(GeneratedMathElements);

		//Updating me count in lines
		bool bStartedWithNewLine = GeneratedMathElements[0]->IsOfType(MathElementV2::FTAMeNewLine::StaticType());
		FTAMeHelpers::CalculateMeCountInLines(this, Path.TreePath[0] - int(bStartedWithNewLine), Path.TreePath[0] + GeneratedMathElements.size());
		if (IsOutsideVisibleArea(Path.TreePath[0] + GeneratedMathElements.size()))
		{
			//Me added before VisibleFrom - VisibleFrom invalid
			//Find the beginning of the line where the last added element is located
			VisibleFrom = FTAMeHelpers::FindLineStartInd(this, Path.TreePath[0] + GeneratedMathElements.size() - 1);
			ActualizeCurrentLine();
		}
		//Add lines from generated elements
		UpdateLinesCount(FTAMeHelpers::GetLinesCount(GeneratedMathElements));
	}
	//Position visible math elements
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
		if (IsOutsideVisibleArea(Path.TreePath[0]))
		{
			//Me remuved before VisibleFrom - VisibleFrom invalid
			//Find the beginning of the line where the last added element is located
			VisibleFrom = FTAMeHelpers::FindLineStartInd(this, Path.TreePath[0] - 1);
			ActualizeCurrentLine();
		}
		ActualizeCurrentLine();
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
	if (Count == 0)
	{
		return;
	}
	VisibleFrom = FTAMeHelpers::ScrollY(this, Count);
	CurrentLine = std::clamp(CurrentLine + Count, 0, LinesCount - 1);
	OnCurrentLineUpdated.Invoke(this);
	ArrangeVisibleElements();
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

bool MathElementV2::FTAMeDocument::IsOutsideVisibleArea(int MeInd)
{
	if ((MeInd > VisibleTo && bTextFillsVisibleArea) || MeInd <= VisibleFrom)
	{
		return true;
	}
	return false;
}

void MathElementV2::FTAMeDocument::UpdateLinesCount(int LinesCountChange)
{
	if (LinesCountChange != 0)
	{
		LinesCount += LinesCountChange;
		OnLinesCountUpdated.Invoke(this);
	}
}

void MathElementV2::FTAMeDocument::ActualizeCurrentLine()
{
	int OldCurrentLine = CurrentLine;
	CurrentLine = FTAMeHelpers::GetLineByMeId(this, VisibleFrom);
	if (CurrentLine != OldCurrentLine)
	{
		OnCurrentLineUpdated.Invoke(this);
	}
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
		//no children-> clear from-to
		VisibleFrom = -1;
		VisibleTo = 0;
	}
	float MaxHorizontalAlignment = FLT_MIN;
	float YOffset = 0;
	float XOffset = 0;
	bTextFillsVisibleArea = false;

	int LineStart = std::max(0, VisibleFrom);
	for (int i = LineStart; i < Children.size(); i++)
	{
		auto NewLine = Children[i]->Cast<MathElementV2::FTAMeNewLine>();
		if (NewLine)
		{
			//if new line
			//lower all elements in line by max hor alignment and YOffset
			float MaxYOffset = VisibleFrom == -1 ? NewLine->GetAbsoluteSize().y : 0;
			for (int j = LineStart; j < i; j++)
			{
				auto LocalPos = Children[j]->GetLocalPosition();
				LocalPos.y += MaxHorizontalAlignment + YOffset;
				MaxYOffset = std::max(MaxYOffset, LocalPos.y + Children[j]->GetAbsoluteSize().y);
				Children[j]->SetLocalPosition(LocalPos);
			}
			//Update line start
			LineStart = i;
			//clear x offset and hor.alignment
			MaxHorizontalAlignment = FLT_MIN;
			XOffset = 0;
			//update YOffset
			YOffset = MaxYOffset;
			//update VisibleTo
			VisibleTo = LineStart;
			if (YOffset >= Height)
			{
				//The next line will be off-screen
				//finish calculation
				bTextFillsVisibleArea = true;
				break;
			}
		}

		//Me is a regular element

		//move element up by hor alignment
		auto HorAlign = Children[i]->GetHorizontalAlignmentOffset();
		TACommonTypes::FTAVector2d NewPos;
		NewPos.y = -HorAlign;
		NewPos.x = XOffset;
		Children[i]->SetLocalPosition(NewPos);
		//cache max hor.align
		MaxHorizontalAlignment = std::max(MaxHorizontalAlignment, HorAlign);
		//Advance X
		XOffset += Children[i]->GetAbsoluteSize().x;

		if (i + 1 == Children.size())
		{
			//if this is the last me
			//set visible to to be the end of doc
			VisibleTo = Children.size();
			//lower all elements in line by max hor alignment and YOffset
			for (int j = LineStart; j <= i; j++)
			{
				auto LocalPos = Children[j]->GetLocalPosition();
				LocalPos.y += MaxHorizontalAlignment + YOffset;
				Children[j]->SetLocalPosition(LocalPos);
			}
		}
	}
}
