// Fill out your copyright notice in the Description page of Project Settings.


#include "Modules/TypedObject.h"


FTATypedObject::FTATypedObject()
{
	SelfShared = std::shared_ptr<FTATypedObject>(this, [](FTATypedObject* obj){});
}

bool FTATypedObject::IsOfType(FType InType) const
{
	return InType == Type || Types.contains(InType);
}