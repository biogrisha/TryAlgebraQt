// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include <vector>
#include <FunctionLibraries/FileHelpers.h>
#include "CommonTypes/MulticastDelegate.h"


class FTAMainModule
{
public:
	FTAMainModule();
	bool NewDocument(EDocumentType Type, const std::wstring& DocumentPath, std::weak_ptr<FTAMathDocumentInfo>& OutDocInfo,const std::shared_ptr<class FTACompatibilityData>& CompatibilityData);
	void SaveSelectDocument(const std::weak_ptr<FTAMathDocumentInfo>& DocInfo);
	void SelectDocument(const std::weak_ptr<FTAMathDocumentInfo>& DocInfo);
	std::vector<std::weak_ptr<FTAMathDocumentInfo>> GetAllDocuments() const;
	void CloseDocument(const std::weak_ptr<FTAMathDocumentInfo>& DocInfo);
	void SaveCurrentDocument() const;
	void SaveDocument(const std::weak_ptr<FTAMathDocumentInfo>& DocInfo) const;
	bool OpenDocument(const std::wstring& DocumentPath, std::weak_ptr<FTAMathDocumentInfo>& OutDocInfo, const std::shared_ptr<class FTACompatibilityData>& CompatibilityData);
	bool OpenOrCreateDocument(std::weak_ptr<FTAMathDocumentInfo>& OutDocInfo, const std::shared_ptr<class FTACompatibilityData>& CompatibilityData, const std::wstring& RelDocumentPath);
	void Compile();
	std::weak_ptr<class FTAActionsManager> GetActionsManager();
	std::weak_ptr<class FTACommandBindings> GetCommandBindings();

private:
	void SetupMathDocument(const FTAMathDocumentInfo* DocInfo);
	void LoadLocalExpressions();
	std::shared_ptr<FTAMathDocumentInfo>* FindDocumentInfo(const std::wstring& DocumentPath);
	std::weak_ptr<FTAMathDocumentInfo> CurrentDocument;
	std::vector<std::shared_ptr<FTAMathDocumentInfo>> Documents;
	std::shared_ptr<FTAActionsManager> ActionsManager;
	std::shared_ptr<class FTAExpressionsModule> ExpressionsModule;
	std::shared_ptr<class FTACommandBindings> CommandBindings;
	std::shared_ptr<class FTACustomTokenization> CustomTokenization;
public:
	FTAMulticastDelegate<> OnDocumentsClosed;
	FTAMulticastDelegate<const std::weak_ptr<FTAMathDocumentInfo>&> OnDocumentSelected;
};
