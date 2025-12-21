// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "BaseMathAction.h"


class FTAShowDebugTree_Ma : public FTABaseMathAction
{
	TYPED_CLASS_META(FTAShowDebugTree_Ma, FTABaseMathAction)
public:
	struct FDependency
	{
		std::weak_ptr<class FTACustomTokenization> CustomTokenization;
	};

	FTAShowDebugTree_Ma(const FDependency& InDependency);
	virtual bool CanPerformAction(const TTypedWeak<class FTAMathDocument>& InDocument) override;
	virtual void OnCantPerform() override;
	void ShowDebugTree();

private:
	std::wstring MeData;
	FDependency Dependency;
};
