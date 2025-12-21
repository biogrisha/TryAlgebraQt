// Fill out your copyright notice in the Description page of Project Settings.


#include "Modules/MathElementsV2/BracketsAdjustComponent.h"
#include "FunctionLibraries/MathElementsHelpers.h"
#include "Modules/MathElementsV2/Me/MeBracket.h"
#include "Modules/MathElementsV2/Me/MeDocument.h"
#include "Modules/MathElementsV2/Me/MeIndex.h"

namespace
{
	std::map<wchar_t, wchar_t> OpenCloseBrackets
	{
		{L'[', L']'},
		{L'(', L')'},
		{L'⟨', L'⟩'},
		{L'{', L'}'},
	};
}

void FTABracketsAdjustComponent::OnComponentAdded()
{
	MeDocument = Host->Cast<MathElementV2::FTAMeDocument>()->GetSelfWeak();
	PreAlignedHandle = MeDocument->PreAligned.AddFunc(this, &FTABracketsAdjustComponent::PreAligned);
}

void FTABracketsAdjustComponent::PreAligned(const MathElementV2::FMathElements& MathElements, int From, int To)
{
	if (MathElements.empty())
	{
		return;
	}
	if (auto Document = MathElements[0]->GetParent()->Cast<MathElementV2::FTAMeDocument>())
	{
		std::function Callable = [this](const MathElementV2::FMathElements& MathElements, int From, int To)
		{
			PreAlignedLine(MathElements, From, To);
		};
		FTAMeHelpers::IterateOverLines(Document, From, To, Callable);
	}
	else
	{
		PreAlignedLine(MathElements, From, To);
	}
}

void FTABracketsAdjustComponent::PreAlignedLine(const MathElementV2::FMathElements& MathElements, int From, int To)
{
	float ExtraHeightCoef = MathElements[0]->GetParent()->GetReferenceScaler() / 6.f;
	std::vector<FBracketInfo> BracketsStack;
	//Iterate over me
	for (int i = From; i < To; ++i)
	{
		MathElementV2::FTAMeBracket* Bracket = MathElements[i]->Cast<MathElementV2::FTAMeBracket>();
		bool bIsIndex = MathElements[i]->IsOfType(MathElementV2::FTAMeIndex::StaticType());
		if (Bracket)
		{
			//Reset brackets custom size
			auto DefaultSize = Bracket->GetDefaultSize();
			Bracket->SetAbsoluteSize(DefaultSize);
			Bracket->SetCustomHorizontalAlignment(DefaultSize.y / 2.f);
			//If found bracket
			wchar_t BracketChar = Bracket->GetChar();
			auto FoundBracket = OpenCloseBrackets.find(BracketChar);
			if (FoundBracket != OpenCloseBrackets.end())
			{
				//If it is open bracket
				//Add to stack
				BracketsStack.push_back(FBracketInfo{FoundBracket->second, Bracket});
			}
			else if (!BracketsStack.empty())
			{
				auto LastBracket = BracketsStack.back();
				if (BracketChar == LastBracket.CloseBracket)
				{
					TACommonTypes::FTAVector2d Size = {Bracket->GetDefaultSize().x, LastBracket.MaxOffsetAbove + LastBracket.MaxOffsetBelow + ExtraHeightCoef};
					float ExtraSize = 0;
					if (LastBracket.MaxOffsetAbove != 0 || LastBracket.MaxOffsetBelow != 0)
					{
						ExtraSize = ExtraHeightCoef / 2.f;
					}
					//Set new size and offset
					LastBracket.Bracket->SetAbsoluteSize(Size);
					LastBracket.Bracket->SetCustomHorizontalAlignment(LastBracket.MaxOffsetAbove + ExtraSize);
					Bracket->SetAbsoluteSize(Size);
					Bracket->SetCustomHorizontalAlignment(LastBracket.MaxOffsetAbove + ExtraSize);
					BracketsStack.pop_back();
				}
			}
		}
		if (!BracketsStack.empty() && !bIsIndex)
		{
			//Cache MaxOffsetAbove and MaxOffsetBelow
			auto& LastBracket = BracketsStack.back();
			float AlignOffset = MathElements[i]->GetHorizontalAlignmentOffset();
			LastBracket.MaxOffsetAbove = (std::max)(AlignOffset, LastBracket.MaxOffsetAbove);
			LastBracket.MaxOffsetBelow = (std::max)(MathElements[i]->GetAbsoluteSize().y - AlignOffset, LastBracket.MaxOffsetBelow);
		}
	}
}
