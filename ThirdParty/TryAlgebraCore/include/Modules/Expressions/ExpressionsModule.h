// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include <vector>

#include "FunctionLibraries/ExpressionsLibraryExtra.h"



class FTAExpressionsModule
{
public:
	FTAExpressionsModule(const std::weak_ptr<class FTACustomTokenization>& InCustomTokenization);
	bool LoadExpressions(std::wstring& FailedPath);
	const std::vector<TAExpressionsLibrary::FTAExpressionsFile>& GetExpressionsFiles() const;
private:
	std::vector<TAExpressionsLibrary::FTAExpressionsFile> ExpressionFiles;
	std::weak_ptr<class FTACustomTokenization> CustomTokenization;
};
