// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include <cstdint>
#include "Modules/CommonTypes/Vector2D.h"
#include "Modules/MathElementsV2/Me/MathElementBaseV2.h"
class FTAVisual
{
public:
	virtual ~FTAVisual() = default;
	virtual void Show(MathElementV2::FTAMathElementBase* Me);
	virtual void Hide(MathElementV2::FTAMathElementBase* Me);
	virtual TACommonTypes::FTAVector2d GetVisualSize(MathElementV2::FTAMathElementBase* me);
};
