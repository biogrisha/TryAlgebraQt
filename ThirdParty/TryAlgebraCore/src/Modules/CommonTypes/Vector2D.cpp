// Fill out your copyright notice in the Description page of Project Settings.


#include "Modules/CommonTypes/Vector2D.h"


TACommonTypes::FTAVector2d TACommonTypes::Max(const TACommonTypes::FTAVector2d& First, const TACommonTypes::FTAVector2d& Second)
{
	TACommonTypes::FTAVector2d Result;
	Result.x = First.x > Second.x ? First.x : Second.x;
	Result.y = First.y > Second.y ? First.y : Second.y;
	return Result;
}
