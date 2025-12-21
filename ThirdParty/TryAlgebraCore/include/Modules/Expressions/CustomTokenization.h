// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "FunctionLibraries/ExpressionsLibrary.h"

class FTACustomTokenization
{
public:
	void UpdateRewritingRules();
	const std::vector<TAExpressionsLibrary::FRewritingRules>& GetRewritingRulesArr();
private:
	std::vector<TAExpressionsLibrary::FRewritingRules> RewritingRulesArr;
};
