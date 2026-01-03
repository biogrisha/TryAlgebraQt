// Fill out your copyright notice in the Description page of Project Settings.


#include "Modules/Visual/VisualBase.h"

void FTAVisual::Show()
{
	bShown = true;
}

void FTAVisual::Hide()
{
	bShown = false;
}

TACommonTypes::FTAVector2d FTAVisual::GetVisualSize()
{
	return TACommonTypes::FTAVector2d();
}
