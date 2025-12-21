// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include <map>

#include "ExpressionsLibrary.h"
#include "Modules/CommonTypes/Vector2D.h"
#include "FunctionLibraries/CommonHelpers.h"

namespace TAExpressionsLibrary
{
	struct FTAExpression
	{
		std::wstring Name;
		std::vector<FSymbolPtr> Parameters;
		std::vector<FSymbols> Forms;
	};

	struct FTAExpressionsFile
	{
		std::wstring FileName;
		std::vector<FTAExpression> Expressions;
	};

	struct FTADebugSymbol
	{
		std::wstring Name;
		std::wstring Info;
		//FVector2d Location;
		std::vector<std::shared_ptr<FTADebugSymbol>> Children;
	};

	std::vector<std::shared_ptr<FTADebugSymbol>> MathTreeToDebug(const FSymbols& MathTree);
	void DebugMathTree(const FSymbols& MathTree);
	void SetupTreePositions(const std::vector<std::shared_ptr<FTADebugSymbol>>& DebugTree, float& LastOffset, int Depth);
	float MeanChildrenX(const std::vector<std::shared_ptr<FTADebugSymbol>>& DebugTree);
	//void CollectDebugData(const std::vector<std::shared_ptr<FTADebugSymbol>>& DebugTree, TArray<FMathDebugNode>& DebugData);

	bool ParseExpressionsFile(const std::wstring& FileContent, const std::vector<FRewritingRules>& RewritingRulesArr, FTAExpressionsFile& OutExpressionsFile);

	template <typename... Args>
	bool IsCharacterRange(const FSymbolPtr& Symbol, const Args&... Character)
	{
		return IsCharacter(Symbol) && CommonHelpers::IsIn(Symbol->Info, Character...);
	}

	//Bindings
	bool ParseBindings(const std::wstring& MathData, std::map<std::wstring, std::wstring>& OutBindings);
	bool IsExpressionStart(const FSymbolPtr& Symbol);
	bool IsExpressionEnd(const FSymbolPtr& Symbol);
}
