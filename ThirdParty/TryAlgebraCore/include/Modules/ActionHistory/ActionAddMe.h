// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "ActionBase.h"
#include "Modules/MathElementsV2/MathElementPath.h"

class FTAActionAddMe : public FTAActionBase
{
	TYPED_CLASS_META(FTAActionAddMe,FTAActionBase);
	public:
	FTAActionAddMe(const TTypedWeak<MathElementV2::FTAMeDocument>& InMeDocument, const FTAMePath& InPath, int InCount);
	virtual void Undo() override;
private:
	FTAMePath Path;
	int Count;
};
