// Fill out your copyright notice in the Description page of Project Settings.


#include "Modules/ActionHistory/ActionReplaceMe.h"

#include "Modules/MathElementsV2/Me/MeDocument.h"

FTAActionReplaceMe::FTAActionReplaceMe(const TTypedWeak<MathElementV2::FTAMeDocument>& InMeDocument, const FTAMePath& InPath, const std::wstring& InMeData, int InCount)
	: FTAActionBase(InMeDocument)
{
	Path = InPath;
	Count = InCount;
	MeData = InMeData;
}

void FTAActionReplaceMe::Undo()
{
	MeDocument->RemoveMathElements(Path, Count);
	MeDocument->AddMathElements(Path, MeData);
}
