// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include <string>
#include "ActionBase.h"
#include "Modules/MathElementsV2/MathElementPath.h"


class FTAActionReplaceMe : public FTAActionBase
{
	TYPED_CLASS_META(FTAActionReplaceMe,FTAActionBase);
public:
	FTAActionReplaceMe(const TTypedWeak<MathElementV2::FTAMeDocument>& InMeDocument, const FTAMePath& InPath, const std::wstring& InMeData, int InCount);
	virtual void Undo() override;
private:
	FTAMePath Path;
	int Count;
	std::wstring MeData;
};
