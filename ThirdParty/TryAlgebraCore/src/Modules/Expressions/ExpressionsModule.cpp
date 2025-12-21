// Fill out your copyright notice in the Description page of Project Settings.


#include "Modules/Expressions/ExpressionsModule.h"

#include "GlobalVariables.h"
#include "FunctionLibraries/ExpressionsLibraryExtra.h"
#include "FunctionLibraries/FileHelpers.h"
#include "Modules/Expressions/CustomTokenization.h"

FTAExpressionsModule::FTAExpressionsModule(const std::weak_ptr<class FTACustomTokenization>& InCustomTokenization)
{
	CustomTokenization = InCustomTokenization;
}

bool FTAExpressionsModule::LoadExpressions(std::wstring& FailedPath)
{
	ExpressionFiles.clear();
	std::wstring Dir = FTAFileSystemHelpers::GetDocumentsPath() + EXPRESSIONS_PATH;
	std::vector<std::wstring> Files = FTAFileSystemHelpers::GetAllFilesInDirectory(Dir, EXPRESSIONS_EXT);
	
	for (const std::wstring& File : Files)
	{
		std::wstring FileContent;
		FTAFileSystemHelpers::ReadFromFile(File, FileContent);
		TAExpressionsLibrary::FTAExpressionsFile ExpressionFile;
		if (ParseExpressionsFile(FileContent,CustomTokenization.lock()->GetRewritingRulesArr(), ExpressionFile))
		{
			std::wstring RelativePath = File;
			FTAFileSystemHelpers::MakeRelativeTo(RelativePath, Dir);
			ExpressionFile.FileName = RelativePath;
			ExpressionFiles.push_back(std::move(ExpressionFile));
		}
		else
		{
			FailedPath = File;
			return false;
		}
	}
	return true;
}

const std::vector<TAExpressionsLibrary::FTAExpressionsFile>& FTAExpressionsModule::GetExpressionsFiles() const
{
	return ExpressionFiles;
}
