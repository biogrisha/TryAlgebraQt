// Fill out your copyright notice in the Description page of Project Settings.


#include "Modules/MathElementsV2/Me/MeCharacter.h"
#include "Modules/Visual/VisualBase.h"

MathElementV2::FTAMeCharacter::FTAMeCharacter(wchar_t InChar)
{
	Char = InChar;
}

std::wstring MathElementV2::FTAMeCharacter::GetParseInfo()
{
	return std::wstring(1,Char);
}

void MathElementV2::FTAMeCharacter::CalculateSize(float AccumulatedScalingFactor)
{
	if(!bScaleFactorApplied)
	{
		FontSize *= AccumulatedScalingFactor;
		AbsoluteSize = Visual->GetVisualSize();
		bScaleFactorApplied = true;
	}
}

uint32_t MathElementV2::FTAMeCharacter::GetFontSize()
{
	return FontSize;
}


