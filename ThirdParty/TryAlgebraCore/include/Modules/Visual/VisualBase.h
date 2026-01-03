// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include <cstdint>
#include "Modules/CommonTypes/Vector2D.h"

class FTAVisual
{
public:
	virtual ~FTAVisual() = default;
	virtual void Show();
	virtual void Hide();
	virtual TACommonTypes::FTAVector2d GetVisualSize();
	bool IsShown() const {return bShown;};
private:
	bool bShown = false;
};
