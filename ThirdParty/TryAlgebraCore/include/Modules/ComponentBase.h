// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include <vector>
#include "TypedObject.h"

class FTAComponentBase : public FTATypedObject
{
	TYPED_CLASS_META(FTAComponentBase, FTATypedObject);
protected:
	virtual void OnComponentAdded(){}
	friend class FTAComponentsHost;
	class FTAComponentsHost* Host = nullptr;
	
};

class FTAComponentsHost	: public FTATypedObject
{
	TYPED_CLASS_META(FTAComponentsHost,FTATypedObject)
public:
	void AddComponent(const std::shared_ptr<FTAComponentBase>& Component);
	FTAComponentBase* GetComponent(FTATypedObject::FType InType);
	
	template <class T, std::enable_if_t<std::_SP_pointer_compatible<T, FTAComponentBase>::value, int> = 0>
	T* GetComponent()
	{
		for (auto& Component : Components)
		{
			if (T* CastedComp = Component->Cast<T>())
			{
				return CastedComp;
			}
		}
		return nullptr;
	}
	
private:
	std::vector<std::shared_ptr<FTAComponentBase>> Components;
};