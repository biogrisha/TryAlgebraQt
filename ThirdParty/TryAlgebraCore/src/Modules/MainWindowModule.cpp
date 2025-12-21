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
	if (FindDocumentInfo(DocumentPath))
	{
		//If document already in memory -> return
		return false;
	}
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

void FTAMainModule::SaveSelectDocument(const std::weak_ptr<FTAMathDocumentInfo>& DocInfo)
{
	if (!CurrentDocument.expired())
	{
		//Save Current doc
		SaveCurrentDocument();
	}
	SelectDocument(DocInfo);
}

void FTAMainModule::SelectDocument(const std::weak_ptr<FTAMathDocumentInfo>& DocInfo)
{
	if (DocInfo.lock() == CurrentDocument.lock())
	{
		return;
	}
	if (!DocInfo.expired())
	{
		if (!CurrentDocument.expired())
		{
			//If there was document-> clear its visuals
			CurrentDocument.lock()->MathDocument->ClearDrawing();
		}
		//Set new current doc
		CurrentDocument = DocInfo;
		//Signal "New doc Selected"
		OnDocumentSelected.Invoke(DocInfo);
		//Redraw it
		CurrentDocument.lock()->MathDocument->Draw();
	}
}

std::vector<std::weak_ptr<FTAMathDocumentInfo>> FTAMainModule::GetAllDocuments() const
{
	std::vector<std::weak_ptr<FTAMathDocumentInfo>> Result;
	for (auto& DocInfo : Documents)
	{
		Result.push_back(DocInfo);
	}
	return Result;
}

void FTAMainModule::CloseDocument(const std::weak_ptr<FTAMathDocumentInfo>& DocInfo)
{
	//Close logic
	//Find Doc index
	int Index = CommonHelpers::Find(Documents, DocInfo.lock());
	if (Index == CommonHelpers::InvalidIndex)
	{
		return;
	}
	if (DocInfo.lock() == CurrentDocument.lock())
	{
		//if closing current doc-> save
		SaveCurrentDocument();
	}
	//Remove from documents
	CommonHelpers::RemoveSwap(Documents, Index);
	if (CurrentDocument.expired())
	{
		//if this was current doc
		//Clamp this index from above
		Index = (std::min)(Index, static_cast<int>(Documents.size() - 1));
		if (Index >= 0)
		{
			//If index still valid -> select document under new index
			SelectDocument(Documents[Index]);
		}
		else
		{
			//If all documents were closed-> signal
			OnDocumentsClosed.Invoke();
		}
	}
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

bool FTAMainModule::OpenDocument(const std::wstring& DocumentPath, std::weak_ptr<FTAMathDocumentInfo>& OutDocInfo, const std::shared_ptr<class FTACompatibilityData>& CompatibilityData)
{
	if (auto Doc = FindDocumentInfo(DocumentPath))
	{
		//If document already in memory -> return it
		OutDocInfo = *Doc;
		return true;
	}
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
				OutDocInfo = Documents.back();
				SetupMathDocument(OutDocInfo.lock().get());
				return true;
			}
		}
	case EDocumentType::Expressions:
		{
			//If document or expressions
			FTAMathDocumentInfo MathDocumentInfo;
			if (FTAFileSystemHelpers::OpenSimpleDocument(DocumentPath, MathDocumentInfo, DocType, CompatibilityData))
			{
				//If succeeded to create document, add to documents array and return this doc
				Documents.push_back(std::make_shared<FTAMathDocumentInfo>(MathDocumentInfo));
				OutDocInfo = Documents.back();
				return true;
			}
		}
	default: ;
	}
	

	return false;
}

void FTAMainModule::Compile()
{
	SaveCurrentDocument();
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
		return OpenDocument(BindingsPath, OutDocInfo, CompatibilityData);
	}
	return NewDocument(EDocumentType::Document, BindingsPath, OutDocInfo, CompatibilityData);
}

std::weak_ptr<class FTAActionsManager> FTAMainModule::GetActionsManager()
{
	return ActionsManager;
}

void FTAMainModule::SaveCurrentDocument() const
{
	SaveDocument(CurrentDocument);
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

std::shared_ptr<FTAMathDocumentInfo>* FTAMainModule::FindDocumentInfo(const std::wstring& DocumentPath)
{
	return CommonHelpers::FindByPredicate(Documents, [&DocumentPath](const std::shared_ptr<FTAMathDocumentInfo>& Element)
	{
		return Element->FilePath == DocumentPath;
	});
}
