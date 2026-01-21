// Fill out your copyright notice in the Description page of Project Settings.


#include "Modules/MathElementsV2/Me/MathElementBaseV2.h"

#include "Modules/MathElementsV2/Me/MeComposite.h"
#include "Modules/Visual/VisualBase.h"


void MathElementV2::FTAMathElementBase::ScaleRecursive(float Height)
{
	Scale(Height);
}

void MathElementV2::FTAMathElementBase::Scale(float ScalingFactor)
{
	DefaultSize *= ScalingFactor;
	AbsoluteSize *= ScalingFactor;
	LocalPosition *= ScalingFactor;
}

float MathElementV2::FTAMathElementBase::GetHorizontalAlignmentOffset() const
{
	return AbsoluteSize.y / 2.f;
}

void MathElementV2::FTAMathElementBase::SetDefaultSize(const TACommonTypes::FTAVector2d& InDefaultSize)
{
	DefaultSize = InDefaultSize;
	AbsoluteSize = DefaultSize;
	InitialHeight = DefaultSize.y;
}

void MathElementV2::FTAMathElementBase::SetAbsoluteSize(const TACommonTypes::FTAVector2d& InSize)
{
	AbsoluteSize = InSize;
}

TACommonTypes::FTAVector2d MathElementV2::FTAMathElementBase::GetAbsolutePosition() const
{
	TACommonTypes::FTAVector2d Result = LocalPosition;
	TTypedWeak<FTAMeComposite> TempParent = Parent;
	while (TempParent.Get() != nullptr)
	{
		Result += TempParent->LocalPosition;
		TempParent = TempParent->GetParent();
	}
	return Result;
}

TACommonTypes::FTAVector2d MathElementV2::FTAMathElementBase::GetLocalPosition() const
{
	return LocalPosition;
}

void MathElementV2::FTAMathElementBase::SetParent(const FTAMeComposite* InParent)
{
	if (!InParent)
	{
		Parent = TTypedWeak<FTAMeComposite>();
		return;
	}
	Parent = InParent->GetSelfWeak();
}

void MathElementV2::FTAMathElementBase::SetVisual(FTAVisual* InVisual)
{
	Visual = InVisual;
}

void MathElementV2::FTAMathElementBase::SetLocalPosition(const TACommonTypes::FTAVector2d& InLocalPosition)
{
	LocalPosition = InLocalPosition;
}

void MathElementV2::FTAMathElementBase::Destroy()
{
	if (Visual && bVisible)
	{
		Visual->Hide(this);
	}
}

void MathElementV2::FTAMathElementBase::CalculateAbsolutePosition()
{
	
}

void MathElementV2::FTAMathElementBase::CalculateSize(float AccumulatedScalingFactor)
{
}

