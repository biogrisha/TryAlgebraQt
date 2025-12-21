// Fill out your copyright notice in the Description page of Project Settings.


#include "Modules/ContextActions/ShowDebugTree_Ma.h"

#include "FunctionLibraries/ExpressionsLibraryExtra.h"
#include "Modules/Expressions/CustomTokenization.h"
#include "Modules/MathDocument/MathDocument.h"

FTAShowDebugTree_Ma::FTAShowDebugTree_Ma(const FDependency& InDependency)
{
	ActionName = L"ShowDebugTree";
	Dependency = InDependency;
}

bool FTAShowDebugTree_Ma::CanPerformAction(const TTypedWeak<class FTAMathDocument>& InDocument)
{
	MeData.clear();
	MeData = InDocument->GetSelectedMeData();
	return !MeData.empty();
}

void FTAShowDebugTree_Ma::OnCantPerform()
{
	MeData.clear();
}	

void FTAShowDebugTree_Ma::ShowDebugTree()
{
	auto MathTree = TAExpressionsLibrary::ParseToMathTree(MeData);
	TAExpressionsLibrary::LogicalParse(MathTree, Dependency.CustomTokenization.lock()->GetRewritingRulesArr());
	TAExpressionsLibrary::DebugMathTree(MathTree);
}
