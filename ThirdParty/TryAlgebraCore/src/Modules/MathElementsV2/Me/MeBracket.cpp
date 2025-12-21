// Fill out your copyright notice in the Description page of Project Settings.


#include "Modules/MathElementsV2/Me/MeBracket.h"

MathElementV2::FTAMeBracket::FTAMeBracket(wchar_t Char)
	: FTAMeCharacter(Char)
{
}

void MathElementV2::FTAMeBracket::SetCustomHorizontalAlignment(float Val)
{
	CustomHorizontalAlignment = Val;
}

float MathElementV2::FTAMeBracket::GetHorizontalAlignmentOffset() const
{
	return CustomHorizontalAlignment;
}
