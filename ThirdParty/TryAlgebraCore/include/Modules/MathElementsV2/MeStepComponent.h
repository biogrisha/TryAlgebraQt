// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Modules/ComponentBase.h"

class FTAMeStepComponent : public FTAComponentBase
{
	TYPED_CLASS_META(FTAMeStepComponent,FTAComponentBase);
public:
	virtual void OnComponentAdded() override;
	//return true if step outside me
	virtual int StepX(int Count, int Index) { return -1; };
	virtual int StepY(int Count, int Index) { return -1; };
};
