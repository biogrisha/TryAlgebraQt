// Fill out your copyright notice in the Description page of Project Settings.


#include "Modules/Visual/VisualBase.h"

void FTAVisual::Show(MathElementV2::FTAMathElementBase* Me)
{
	Me->bVisible = true;
}

void FTAVisual::Hide(MathElementV2::FTAMathElementBase* Me)
{
	Me->bVisible = false;
}

TACommonTypes::FTAVector2d FTAVisual::GetVisualSize(MathElementV2::FTAMathElementBase* me)
{
	return TACommonTypes::FTAVector2d();
}
