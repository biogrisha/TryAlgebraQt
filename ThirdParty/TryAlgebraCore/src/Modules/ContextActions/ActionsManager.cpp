// Fill out your copyright notice in the Description page of Project Settings.


#include "Modules/ContextActions/ActionsManager.h"

#include "Modules/ContextActions/Expressions_Ma.h"
#include "Modules/ContextActions/Matrix_Ma.h"
#include "Modules/ContextActions/ShowDebugTree_Ma.h"


std::vector<TTypedWeak<class FTABaseMathAction>> FTAActionsManager::GetAvailableActions(const TTypedWeak<class FTAMathDocument>& InDocument)
{
	std::vector<TTypedWeak<class FTABaseMathAction>> AvailableActions;
	for (auto& Action : Actions)
	{
		if (Action->CanPerformAction(InDocument))
		{
			AvailableActions.push_back(Action->GetSelfWeak());
		}
		else
		{
			Action->OnCantPerform();
		}
	}
	return AvailableActions;
}

FTAActionsManager::FTAActionsManager(const FDependency& InDependency)
{
	Dependency = InDependency;
	Actions.push_back(FTAMatrix_Ma::MakeTypedShared());
	Actions.push_back(FTAExpressions_Ma::MakeTypedShared(FTAExpressions_Ma::FDependency(
		Dependency.ExpressionsModule,
		Dependency.CustomTokenization)));
	Actions.push_back(FTAShowDebugTree_Ma::MakeTypedShared(FTAShowDebugTree_Ma::FDependency(
		Dependency.CustomTokenization)));
}
