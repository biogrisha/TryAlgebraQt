// Fill out your copyright notice in the Description page of Project Settings.


#include "Modules/MathElementsV2/Me/MeContainer.h"

#include "FunctionLibraries/MeDefinition.h"
#include "FunctionLibraries/MathElementsHelpers.h"
#include "Modules/MathElementsV2/Me/MeDocument.h"

MathElementV2::FTAMeContainer::FTAMeContainer()
{
	AbsoluteSize = { 10,10 };
}

void MathElementV2::FTAMeContainer::ArrangeChildren()
{
	float MinY = 0;
	float Offset = 0;
	FTAMeHelpers::AlignElementsVertically(Children,MinY,Offset);
	FTAMeHelpers::ArrangeInLine(Children);
}

std::wstring MathElementV2::FTAMeContainer::GetParseInfo()
{
	std::wstring Slash = L"\\";
	return Slash + GetName() + Slash + Slash;
}

void MathElementV2::FTAMeContainer::SetOwnerDocument(const TTypedWeak<FTAMeDocument>& InDocument)
{
	OwnerDocument = InDocument;
}

std::wstring MathElementV2::FTAMeContainer::GetName()
{
	return FTAMeDefinition::ContainerName;
}
