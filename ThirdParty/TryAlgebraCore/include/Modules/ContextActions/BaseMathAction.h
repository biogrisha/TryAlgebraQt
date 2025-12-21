// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include <string>

#include "Modules/TypedObject.h"

class FTABaseMathAction : public FTATypedObject
{
	TYPED_CLASS_META(FTABaseMathAction, FTATypedObject)
public:
	virtual bool CanPerformAction(const TTypedWeak<class FTAMathDocument>& InDocument) { return false; };

	virtual void OnCantPerform()
	{
	};
	std::wstring GetActionName();

protected:
	std::wstring ActionName;
};
