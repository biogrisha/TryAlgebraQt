// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include <string>
#include "ActionBase.h"
#include "Modules/MathElementsV2/MathElementPath.h"

class FTAActionRemoveMe : public FTAActionBase
{
	TYPED_CLASS_META(FTAActionRemoveMe,FTAActionBase);
public:
	FTAActionRemoveMe(const TTypedWeak<MathElementV2::FTAMeDocument>& InMeDocument, const FTAMePath& InPath, const std::wstring& InMeData);
	virtual void Undo() override;
private:
	FTAMePath Path;
	std::wstring MeData;
};
