// Fill out your copyright notice in the Description page of Project Settings.


#include "Modules/ComponentBase.h"


void FTAComponentsHost::AddComponent(const std::shared_ptr<FTAComponentBase>& Component)
{
	Component->Host = this;
	Components.push_back(Component);
	Component->OnComponentAdded();
}

FTAComponentBase* FTAComponentsHost::GetComponent(FTATypedObject::FType InType)
{
	for (const std::shared_ptr<FTAComponentBase>& Component : Components)
	{
		if (Component->IsOfType(InType))
		{
			return Component.get();
		}
	}
	return nullptr;
}