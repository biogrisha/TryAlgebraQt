// Fill out your copyright notice in the Description page of Project Settings.


#include "Modules/ActionHistory/ActionRemoveMe.h"

#include "FunctionLibraries/MathElementsHelpers.h"
#include "Modules/MathElementsV2/Me/MeDocument.h"

FTAActionRemoveMe::FTAActionRemoveMe(const TTypedWeak<MathElementV2::FTAMeDocument>& InMeDocument, const FTAMePath& InPath, const std::wstring& InMeData)
	: FTAActionBase(InMeDocument)
{
	Path = InPath;
	MeData = InMeData;
}

void FTAActionRemoveMe::Undo()
{
	MeDocument->AddMathElements(Path, MeData);
}
