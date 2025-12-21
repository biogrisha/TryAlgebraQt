// Fill out your copyright notice in the Description page of Project Settings.


#include "Modules/Expressions/CustomTokenization.h"

#include "GlobalVariables.h"
#include "FunctionLibraries/ExpressionsLibraryExtra.h"
#include "FunctionLibraries/FileHelpers.h"


void FTACustomTokenization::UpdateRewritingRules()
{
	RewritingRulesArr.clear();
	RewritingRulesArr.resize(2);
	std::wstring FileContent;
	//Parse Tokenization0
	FTAFileSystemHelpers::ReadFromFile(FTAFileSystemHelpers::GetDocumentsPath() + TOKENIZATION0_PATH, FileContent);
	if (!FileContent.empty())
	{
		TAExpressionsLibrary::ParseRewritingRules(FileContent, RewritingRulesArr[0], &TAExpressionsLibrary::LogicalParse0);
	}
	//Parse Tokenization1
	FileContent.clear();
	FTAFileSystemHelpers::ReadFromFile(FTAFileSystemHelpers::GetDocumentsPath() + TOKENIZATION1_PATH, FileContent);
	if (!FileContent.empty())
	{
		TAExpressionsLibrary::ParseRewritingRules(FileContent, RewritingRulesArr[1], &TAExpressionsLibrary::LogicalParse1);
	}
}

const std::vector<TAExpressionsLibrary::FRewritingRules>& FTACustomTokenization::GetRewritingRulesArr()
{
	return RewritingRulesArr;
}
