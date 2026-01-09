// Fill out your copyright notice in the Description page of Project Settings.


#include "Modules/MainWindowModule.h"

#include "GlobalVariables.h"
#include "FunctionLibraries/CommonHelpers.h"
#include "FunctionLibraries/FileHelpers.h"
#include "Modules/ContextActions/ActionsManager.h"
#include "Modules/MathDocument/MathDocument.h"
#include "Modules/MathElementsV2/Me/MeDocument.h"
#include <filesystem>

#include "Modules/Expressions/CustomTokenization.h"
#include "Modules/Expressions/ExpressionsComponent.h"
#include "Modules/Expressions/ExpressionsModule.h"
#include "Modules/MathDocument/CommandBindings.h"
#include "Modules/Search/HighlightSameAsSelected.h"
namespace fs = std::filesystem;
FTAMainModule::FTAMainModule()
{
	//Command bindings
	CommandBindings = std::make_shared<FTACommandBindings>();
	CommandBindings->UpdateBindings();
	//Custom tokenization
	CustomTokenization = std::make_shared<FTACustomTokenization>();
	CustomTokenization->UpdateRewritingRules();
	//Expression module
	ExpressionsModule = std::make_shared<FTAExpressionsModule>(CustomTokenization);
	std::wstring FailedPath;
	ExpressionsModule->LoadExpressions(FailedPath);
	LoadLocalExpressions();
	//Actions manager
	FTAActionsManager::FDependency AmDep;
	AmDep.ExpressionsModule = ExpressionsModule;
	AmDep.CustomTokenization = CustomTokenization;
	ActionsManager = std::make_shared<FTAActionsManager>(AmDep);
}

bool FTAMainModule::NewDocument(EDocumentType Type, const std::wstring& DocumentPath, std::weak_ptr<FTAMathDocumentInfo>& OutDocInfo,const std::shared_ptr<FTACompatibilityData>& CompatibilityData)
{
	if (Type == EDocumentType::Document)
	{
		//If document
		//Create new math document
		auto DocumentPathTemp = DocumentPath;
		FTAFileSystemHelpers::WriteToFile(DocumentPathTemp, DOCUMENTS_EXT);
		//Open newly created document
		FTAMathDocumentInfo MathDocumentInfo;
		if (FTAFileSystemHelpers::OpenSimpleDocument(DocumentPathTemp, MathDocumentInfo, Type, CompatibilityData))
		{
			//If succeeded, add to documents array and return this doc
			Documents.push_back(std::make_shared<FTAMathDocumentInfo>(MathDocumentInfo));
			OutDocInfo = Documents.back();
			SetupMathDocument(OutDocInfo.lock().get());
		}
	}
	else if (Type == EDocumentType::Expressions)
	{
		//If Expressions
		//Create new math document
		auto DocumentPathTemp = DocumentPath;
		FTAFileSystemHelpers::WriteToFile(DocumentPathTemp, EXPRESSIONS_EXT);
		//Open newly created document
		FTAMathDocumentInfo MathDocumentInfo;
		if (FTAFileSystemHelpers::OpenSimpleDocument(DocumentPathTemp, MathDocumentInfo, Type, CompatibilityData))
		{
			//If succeeded, add to documents array and return this doc
			Documents.push_back(std::make_shared<FTAMathDocumentInfo>(MathDocumentInfo));
			OutDocInfo = Documents.back();
		}
	}
	return true;
}

const std::vector<std::shared_ptr<FTAMathDocumentInfo>>& FTAMainModule::GetAllDocuments() const
{
	return Documents;
}

void FTAMainModule::CloseDocument(int Ind)
{
	Documents.erase(Documents.begin() + Ind);
}

void FTAMainModule::SaveDocument(const std::weak_ptr<FTAMathDocumentInfo>& DocInfo) const
{
	if (CurrentDocument.expired())
	{
		return;
	}
	if (CurrentDocument.lock()->bCanSave)
	{
		if (auto DocInfoPtr = DocInfo.lock())
		{
			FTAFileSystemHelpers::WriteToFile(DocInfoPtr->FilePath, FTAFileSystemHelpers::ToExtension(DocInfoPtr->Type), DocInfoPtr->MathDocument->GetMeData());
		}
	}
}

bool FTAMainModule::OpenDocument(const std::wstring& DocumentPath, const std::shared_ptr<class FTACompatibilityData>& CompatibilityData)
{
	fs::path Path = {DocumentPath};
	if (!fs::exists(Path))
	{
		return false;
	}
	std::wstring Extension = Path.extension().wstring();
	
	switch (auto DocType = FTAFileSystemHelpers::ToDocumentType(Extension)) {
	case EDocumentType::Document:
		{
			//If document or expressions
			FTAMathDocumentInfo MathDocumentInfo;
			if (FTAFileSystemHelpers::OpenSimpleDocument(DocumentPath, MathDocumentInfo, DocType, CompatibilityData))
			{
				//If succeeded to create document, add to documents array and return this doc
				Documents.push_back(std::make_shared<FTAMathDocumentInfo>(MathDocumentInfo));
				auto DocInfo = Documents.back();
				SetupMathDocument(DocInfo.get());
				return true;
			}
		}
		break;
	case EDocumentType::Expressions:
		{
			//If document or expressions
			FTAMathDocumentInfo MathDocumentInfo;
			if (FTAFileSystemHelpers::OpenSimpleDocument(DocumentPath, MathDocumentInfo, DocType, CompatibilityData))
			{
				//If succeeded to create document, add to documents array and return this doc
				Documents.push_back(std::make_shared<FTAMathDocumentInfo>(MathDocumentInfo));
				return true;
			}
		}
		break;
	default: ;
	}
	

	return false;
}

void FTAMainModule::Compile()
{
	//TODO: need to save modified files
	CommandBindings->UpdateBindings();
	CustomTokenization->UpdateRewritingRules();
	std::wstring FailedPath;
	ExpressionsModule->LoadExpressions(FailedPath);
	LoadLocalExpressions();
}

bool FTAMainModule::OpenOrCreateDocument(std::weak_ptr<FTAMathDocumentInfo>& OutDocInfo, const std::shared_ptr<class FTACompatibilityData>& CompatibilityData, const std::wstring& RelDocumentPath)
{
	auto BindingsPath = FTAFileSystemHelpers::GetDocumentsPath() + RelDocumentPath;
	fs::path Path = {BindingsPath};
	if (fs::exists(Path))
	{
		//return OpenDocument(BindingsPath, OutDocInfo, CompatibilityData);
	}
	return NewDocument(EDocumentType::Document, BindingsPath, OutDocInfo, CompatibilityData);
}

std::weak_ptr<class FTAActionsManager> FTAMainModule::GetActionsManager()
{
	return ActionsManager;
}

std::weak_ptr<class FTACommandBindings> FTAMainModule::GetCommandBindings()
{
	return CommandBindings;
}

void FTAMainModule::SetupMathDocument(const FTAMathDocumentInfo* DocInfo)
{
	//Add a File expressions component
	auto ExprComp = FTAExpressionsComponent::MakeTypedShared(DocInfo->FilePath, CustomTokenization);
	ExprComp->LoadExpressions();
	DocInfo->MathDocument->AddComponent(ExprComp);
	//Add Highlight as Selected component
	auto HighlightAsSelected = FTAHighlightSameAsSelected::MakeTypedShared();
	DocInfo->MathDocument->AddComponent(HighlightAsSelected);
}

void FTAMainModule::LoadLocalExpressions()
{
	for (auto DocInfo : Documents)
	{
		if (auto ExprComp = DocInfo->MathDocument->GetComponent<FTAExpressionsComponent>())
		{
			ExprComp->LoadExpressions();
		}
	}
}

int FTAMainModule::FindDocumentInd(const std::wstring& DocumentPath)
{

	for (int i = 0; i < Documents.size(); i++)
	{
		if (Documents[i]->FilePath == DocumentPath)
		{
			return i;
		}
	}
	return -1;
}
