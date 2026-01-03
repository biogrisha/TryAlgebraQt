// Fill out your copyright notice in the Description page of Project Settings.


#include "Modules/MathElementsV2/Me/MeNewLine.h"

MathElementV2::FTAMeNewLine::FTAMeNewLine()
	:Super(L'\n')
{
	SetDefaultSize({0,1});
}

void MathElementV2::FTAMeNewLine::SetMinY(float Val)
{
	MinY = Val;
}

void MathElementV2::FTAMeNewLine::CalculateSize(float AccumulatedScalingFactor)
{
	AbsoluteSize = DefaultSize;
}
