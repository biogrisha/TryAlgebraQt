// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Me/MathElementBaseV2.h"
#include "Modules/ComponentBase.h"
#include "Modules/CommonTypes/MulticastDelegate.h"


namespace MathElementV2
{
	class FTAMeDocument;
}

class FTAMeIndexAdjustComponent : public FTAComponentBase
{
	TYPED_CLASS_META(FTAMeIndexAdjustComponent,FTAComponentBase);
protected:
	virtual void OnComponentAdded() override;
	void PreAligned(const MathElementV2::FMathElements& MathElements, int From, int To);

	TTypedWeak<MathElementV2::FTAMeDocument> MeDocument;
	FTAMulticastDelegate<const MathElementV2::FMathElements&, int, int>::HndlPtr PreAlignedHandle;
};
