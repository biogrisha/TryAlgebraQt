// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Me/MathElementBaseV2.h"
#include "Modules/ComponentBase.h"
#include "Modules/CommonTypes/MulticastDelegate.h"

namespace MathElementV2
{
	class FTAMeBracket;
	class FTAMeDocument;
}

/**
 * 
 */
class FTABracketsAdjustComponent : public FTAComponentBase
{
	TYPED_CLASS_META(FTABracketsAdjustComponent, FTAComponentBase)
protected:
	struct FBracketInfo
	{
		wchar_t CloseBracket;
		MathElementV2::FTAMeBracket* Bracket;
		//Offset above alignment line
		float MaxOffsetAbove = 0.f;
		//Offset below alignment line
		float MaxOffsetBelow = 0.f;
	};
	virtual void OnComponentAdded() override;
	void PreAligned(const MathElementV2::FMathElements& MathElements, int From, int To);
	void PreAlignedLine(const MathElementV2::FMathElements& MathElements, int From, int To);
	TTypedWeak<MathElementV2::FTAMeDocument> MeDocument;
	FTAMulticastDelegate<const MathElementV2::FMathElements&, int, int>::HndlPtr PreAlignedHandle;
};
