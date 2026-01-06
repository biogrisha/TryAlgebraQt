// Fill out your copyright notice in the Description page of Project Settings.


#include "Modules/MathElementsV2/Me/MeFromTo.h"
#include "Modules/Visual/VisualBase.h"
#include "FunctionLibraries/MeDefinition.h"
#include "FunctionLibraries/MathElementsHelpers.h"


MathElementV2::FTAMeFromTo::FTAMeFromTo(const std::wstring& InSymbol)
{
	Symbol = InSymbol;
	SetDefaultSize({1.f, 1.5f});
	AccumulatedScalingFactor = 0.7f;
}

const std::wstring& MathElementV2::FTAMeFromTo::GetSymbol() const
{
	return Symbol;
}

const TACommonTypes::FTAVector2d& MathElementV2::FTAMeFromTo::GetSymbolPosition() const
{
	return SymbolPosition;
}

std::wstring MathElementV2::FTAMeFromTo::GetName()
{
	return FTAMeDefinition::FromToName;
}

int32_t MathElementV2::FTAMeFromTo::GetFontSize()
{
	return FontSize;
}

float MathElementV2::FTAMeFromTo::GetScalingFactor(int ChildIndex)
{
	return GetReferenceScaler() / 1.5f;
}

std::wstring MathElementV2::FTAMeFromTo::GetParseInfo()
{
	std::wstring Slash = L"\\";
	return Slash + GetName() + Slash + Symbol + Slash;
}

float MathElementV2::FTAMeFromTo::GetHorizontalAlignmentOffset() const
{
	return GetAbsoluteSize().y / 2.f;
}

void MathElementV2::FTAMeFromTo::CalculateSize(float InAccumulatedScalingFactor)
{
	if(!bScaleFactorApplied)
	{
		FontSize *= InAccumulatedScalingFactor;
		SymbolSize = Visual->GetVisualSize();
		DefaultSize = SymbolSize;
		AccumulatedScalingFactor *= InAccumulatedScalingFactor;
		bScaleFactorApplied = true;
	}
	for (auto Me : Children)
	{
		Me->CalculateSize(AccumulatedScalingFactor);
	}
	CalculateCompSize();
}

void MathElementV2::FTAMeFromTo::ArrangeChildren()
{
	if (Children.size() < 2)
	{
		return;
	}
	//Align children horizontally and gt their Center X
	float Center = FTAMeHelpers::AlignHorizontally(Children);
	float HalfDefaultWidth = SymbolSize.x / 2.f;
	if (Center < HalfDefaultWidth)
	{
		//If Symbol Is wider than children, move them to the right so that Symbol could fit
		FTAMeHelpers::MoveMathElementsInX(Children,HalfDefaultWidth - Center);
		Center = HalfDefaultWidth;
	}
	auto Pos1 = Children[0]->GetLocalPosition();
	Pos1.y = 0;
	Children[0]->SetLocalPosition(Pos1);
	auto Pos2 = Children[1]->GetLocalPosition();
	Pos2.y = Children[0]->GetAbsoluteSize().y + SymbolSize.y;
	Children[1]->SetLocalPosition(Pos2);
	
	SymbolPosition = {Center - HalfDefaultWidth, Children[0]->GetAbsoluteSize().y};
	SymbolPosition += GetPadding();
}

void MathElementV2::FTAMeFromTo::Scale(float ScalingFactor)
{
	FTAMeComposite::Scale(ScalingFactor);
	SymbolPosition *= ScalingFactor;
}

