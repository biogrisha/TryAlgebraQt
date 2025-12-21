// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include <vector>
#include "Modules/TypedObject.h"

class FTAActionsManager
{
public:
	struct FDependency
	{
		std::weak_ptr<class FTAExpressionsModule> ExpressionsModule;
		std::weak_ptr<class FTACustomTokenization> CustomTokenization;
	};
	FTAActionsManager(const FDependency& InDependency);
	std::vector<TTypedWeak<class FTABaseMathAction>> GetAvailableActions(const TTypedWeak<class FTAMathDocument>& InDocument);

private:
	FDependency Dependency;
	std::vector<std::shared_ptr<class FTABaseMathAction>> Actions;
};
