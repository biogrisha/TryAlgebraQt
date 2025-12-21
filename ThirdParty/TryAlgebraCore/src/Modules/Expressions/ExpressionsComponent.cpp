// Fill out your copyright notice in the Description page of Project Settings.


#include "Modules/Expressions/ExpressionsComponent.h"

#include "GlobalVariables.h"
#include "FunctionLibraries/FileHelpers.h"
#include "Modules/Expressions/CustomTokenization.h"


FTAExpressionsComponent::FTAExpressionsComponent(const std::wstring& InFilename, const std::weak_ptr<class FTACustomTokenization>& InCustomTokenization)
{
	Expressions.FileName = InFilename;
	FTAFileSystemHelpers::MakeRelativeTo(Expressions.FileName
	                                     , FTAFileSystemHelpers::GetDocumentsPath() + DOCUMENTS_PATH);
	Filename = InFilename;
	CustomTokenization = InCustomTokenization;
}

void FTAExpressionsComponent::LoadExpressions()
{
	std::wstring FileContent;
	FTAFileSystemHelpers::ReadFromFile(Filename, FileContent);
	ParseExpressionsFile(FileContent, CustomTokenization.lock()->GetRewritingRulesArr(), Expressions);
}

const TAExpressionsLibrary::FTAExpressionsFile& FTAExpressionsComponent::GetExpressions() const
{
	return Expressions;
}

