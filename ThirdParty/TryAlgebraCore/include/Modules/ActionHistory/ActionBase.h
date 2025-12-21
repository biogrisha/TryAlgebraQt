// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Modules/TypedObject.h"

namespace MathElementV2
{
	class FTAMeDocument;
}

class FTAActionBase : public FTATypedObject
{
	TYPED_CLASS_META(FTAActionBase,FTATypedObject);
public:
	FTAActionBase(const TTypedWeak<MathElementV2::FTAMeDocument>& InMeDocument);
	virtual void Undo(){};
protected:
	TTypedWeak<MathElementV2::FTAMeDocument> MeDocument;
};
