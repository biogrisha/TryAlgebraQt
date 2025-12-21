// Fill out your copyright notice in the Description page of Project Settings.


#include "Modules/MathElementsV2/Me/MeCharacter.h"

MathElementV2::FTAMeCharacter::FTAMeCharacter(wchar_t InChar)
{
	Char = InChar;
}

std::wstring MathElementV2::FTAMeCharacter::GetParseInfo()
{
	return std::wstring(1,Char);
}

