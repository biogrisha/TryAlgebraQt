// Fill out your copyright notice in the Description page of Project Settings.


#include "Modules/MathElementsV2/Me/MeSpecialSymbol.h"

#include "FunctionLibraries/MeDefinition.h"

MathElementV2::FTAMeSpecialSymbol::FTAMeSpecialSymbol(const std::wstring& InName)
{
	SymbolName = InName;
}

std::wstring MathElementV2::FTAMeSpecialSymbol::GetParseInfo()
{
	std::wstring Slash = L"\\";
	return Slash + GetName() + Slash + GetSymbolName() + Slash;
}

const std::wstring& MathElementV2::FTAMeSpecialSymbol::GetSymbolName()
{
	return SymbolName;
}

std::wstring MathElementV2::FTAMeSpecialSymbol::GetName()
{
	return FTAMeDefinition::SpecSymbolName;
}
