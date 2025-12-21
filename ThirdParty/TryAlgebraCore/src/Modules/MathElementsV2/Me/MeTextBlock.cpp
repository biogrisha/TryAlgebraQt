// Fill out your copyright notice in the Description page of Project Settings.


#include "Modules/MathElementsV2/Me/MeTextBlock.h"

#include "FunctionLibraries/MeDefinition.h"

MathElementV2::FTAMeTextBlock::FTAMeTextBlock()
{
	Padding = {0.1f,0.1f};
}

std::wstring MathElementV2::FTAMeTextBlock::GetParseInfo()
{
	std::wstring Slash = L"\\";
	return Slash + GetName() + Slash + Slash;
}

void MathElementV2::FTAMeTextBlock::ArrangeChildren()
{
	Children[0]->SetLocalPosition({0,0});
}

std::wstring MathElementV2::FTAMeTextBlock::GetName()
{
	return FTAMeDefinition::TextBlockName;
}
