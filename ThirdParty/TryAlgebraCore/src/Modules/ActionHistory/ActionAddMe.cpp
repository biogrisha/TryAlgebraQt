// Fill out your copyright notice in the Description page of Project Settings.


#include "Modules/ActionHistory/ActionAddMe.h"

#include "Modules/MathElementsV2/Me/MeDocument.h"

FTAActionAddMe::FTAActionAddMe(const TTypedWeak<MathElementV2::FTAMeDocument>& InMeDocument, const FTAMePath& InPath, int InCount) : FTAActionBase(InMeDocument)
{
	Path = InPath;
	Count = InCount;
}

void FTAActionAddMe::Undo()
{
	MeDocument->RemoveMathElements(Path, Count);
}
