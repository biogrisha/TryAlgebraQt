// Fill out your copyright notice in the Description page of Project Settings.


#include "Modules/MathElementsV2/MeIndexAdjustComponent.h"

#include "Modules/MathElementsV2/Me/MeDocument.h"
#include "Modules/MathElementsV2/Me/MeIndex.h"
#include "Modules/MathElementsV2/Me/MeNewLine.h"


void FTAMeIndexAdjustComponent::OnComponentAdded()
{
	MeDocument = Host->Cast<MathElementV2::FTAMeDocument>()->GetSelfWeak();
	//PreAlignedHandle = MeDocument->PreAligned.AddFunc(this, &FTAMeIndexAdjustComponent::PreAligned);
}

void FTAMeIndexAdjustComponent::PreAligned(const MathElementV2::FMathElements& MathElements, int From, int To)
{
	if (MathElements.empty())
	{
		return;
	}
	MathElementV2::FMathElementPtr LastMeNotIndex;
	if (From == 0)
	{
		if (auto Index = MathElements[0]->Cast<MathElementV2::FTAMeIndex>())
		{
			Index->UseCustomParams(false);
		}
		else
		{
			LastMeNotIndex = MathElements[0];
		}
	}
	for (int i = (std::max)(From, 1); i < To; i++)
	{
		if (auto Index = MathElements[i]->Cast<MathElementV2::FTAMeIndex>())
		{
			//If found index
			//Get Prev Me to determine how to arrange
			if (!LastMeNotIndex || LastMeNotIndex->IsOfType(MathElementV2::FTAMeNewLine::StaticType()))
			{
				Index->UseCustomParams(false);
				continue;
			}
			Index->UseCustomParams(true);
			float Offset = Index->GetChildren()[0]->GetDefaultSize().y / 2.f;
			if (Index->GetIndexType() == MathElementV2::FTAMeIndex::EType::Both)
			{
				float PrevHeight = LastMeNotIndex->GetAbsoluteSize().y;
				float SuperscriptHeight = Index->GetChildren()[0]->GetAbsoluteSize().y;
				//Align Index s.t superscript at the right place
				Index->SetCustomAlign(LastMeNotIndex->GetHorizontalAlignmentOffset() + SuperscriptHeight - Offset);
				//then lower subscript
				Index->SetCustomSubscriptOffset(PrevHeight - 3 * Offset);
			}
			else if (Index->GetIndexType() == MathElementV2::FTAMeIndex::EType::Superscript)
			{
				float SuperscriptHeight = Index->GetChildren()[0]->GetAbsoluteSize().y;
				//Align elements s.t superscript at the right place
				Index->SetCustomAlign(LastMeNotIndex->GetHorizontalAlignmentOffset() + SuperscriptHeight - Offset);
			}
			else
			{
				float PrevHeight = LastMeNotIndex->GetAbsoluteSize().y;
				//Align elements s.t subscript at the right place
				Index->SetCustomAlign(LastMeNotIndex->GetHorizontalAlignmentOffset() - PrevHeight + Offset * 2);
			}
		}
		else
		{
			LastMeNotIndex = MathElements[i];
		}
	}
}
