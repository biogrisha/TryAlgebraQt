// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include <memory>
#include <string>
#include <vector>
enum class EDocumentType
{
	Undefined,
	Document,
	Expressions,
	Script,

};
struct FTAMathDocumentInfo
{
	std::shared_ptr<class FTAMathDocument> MathDocument;
	EDocumentType Type = EDocumentType::Document;
	std::wstring FilePath;
	std::wstring FileName;
	bool bCanSave = true;
	bool operator==(const FTAMathDocumentInfo& Other) const
	{
		return FilePath == Other.FilePath;
	}
};

struct FTAFileSystemHelpers
{	
	//New
	
	//static bool SaveFileDialogue(const std::wstring& InDirectory, std::wstring& OutFile, const TSharedPtr<SWidget>& ParentWidget, const wchar_t* Filter);
	//static bool OpenFileDialogue(const std::wstring& InDirectory, std::wstring& OutFile, const TSharedPtr<SWidget>& ParentWidget, const wchar_t* Filter);
	static std::vector<wchar_t> GetFileDialogueFilter(const std::vector<std::wstring>& ExtensionNames, const std::vector<std::vector<std::wstring>>& Pattern);
	static std::wstring GetDocumentsPath();
	static bool WriteToFile(std::wstring& FileName, const std::wstring& Extension, const std::wstring& FileContents = L"");
	static bool ReadFromFile(const std::wstring& InPath, std::wstring& OutFile);
	static std::wstring ToExtension(EDocumentType Type);
	static EDocumentType ToDocumentType(const std::wstring& Extension);
	//Document helpers

	static bool OpenSimpleDocument(const std::wstring& InPath, FTAMathDocumentInfo& DocumentInfo, EDocumentType Type,const std::shared_ptr<class FTACompatibilityData>& CompatibilityData);
	static std::vector<std::wstring> GetAllFilesInDirectory(const std::wstring& InDirectory, const std::wstring& InExt);
	static std::wstring& MakeRelativeTo(std::wstring& Path, const std::wstring& RootDir);
};
