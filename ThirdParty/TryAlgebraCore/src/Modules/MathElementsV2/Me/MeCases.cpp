// Fill out your copyright notice in the Description page of Project Settings.


#include "Modules/MathElementsV2/Me/MeCases.h"

#include "FunctionLibraries/CommonHelpers.h"
#include "FunctionLibraries/MathElementsHelpers.h"
#include "FunctionLibraries/MeDefinition.h"


MathElementV2::FTAMeCases::FTAMeCases(const std::wstring& Info)
	:   MathElementV2::FTAMeMatrix(EditInfo(Info))
{
	auto Params = CommonHelpers::SplitStr(Info, L',');
	Bracket = Params.back()[0];
	bCenterContent = false;
}

void MathElementV2::FTAMeCases::ArrangeChildren()
{
	FTAMeMatrix::ArrangeChildren();
	FTAMeHelpers::MoveMathElementsInX(Children, SymbolWidth);
}

std::wstring MathElementV2::FTAMeCases::GetParseInfo()
{
	std::wstring Slash = L"\\";
	return Slash + GetName() + Slash
		+ FTAMeDefinition::GetCasesInfo(Rows, Columns, std::wstring(1, Bracket)) + Slash;
}

std::wstring MathElementV2::FTAMeCases::EditInfo(const std::wstring& InInfo)
{
	auto Params = CommonHelpers::SplitStr(InInfo, L',');
	return Params[0] + L"," + Params[1];
}

void MathElementV2::FTAMeCases::SetSymbolWidth(float InWidth)
{
	SymbolWidth = InWidth;
}

float MathElementV2::FTAMeCases::GetSymbolWidth() const
{
	return SymbolWidth;
}

wchar_t MathElementV2::FTAMeCases::GetBracket() const
{
	return Bracket;
}

std::wstring MathElementV2::FTAMeCases::GetName()
{
	return FTAMeDefinition::CasesName;
}
