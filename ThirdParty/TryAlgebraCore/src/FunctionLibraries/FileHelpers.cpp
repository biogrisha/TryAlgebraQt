// Fill out your copyright notice in the Description page of Project Settings.


#include "FunctionLibraries/FileHelpers.h"

#include <array>
#include <codecvt>
#include <locale>
#include <string>
#include "Windows.h"
#include "Shlobj_core.h"
#include <filesystem>
#include <fstream>

#include "FunctionLibraries/CommonHelpers.h"
#include "Modules/MathDocument/MathDocument.h"
#include <GlobalVariables.h>

std::vector<wchar_t> FTAFileSystemHelpers::GetFileDialogueFilter(const std::vector<std::wstring>& ExtensionNames, const std::vector<std::vector<std::wstring>>& Pattern)
{
	if (ExtensionNames.size() != Pattern.size())
	{
		return {};
	}
	std::vector<wchar_t> Filter;
	for (int i = 0; i < ExtensionNames.size(); i++)
	{
		Filter.insert(Filter.end(), ExtensionNames[i].begin(), ExtensionNames[i].end());
		Filter.push_back(L'\0');
		for (int j = 0; j < Pattern[i].size(); j++)
		{
			Filter.insert(Filter.end(), Pattern[i][j].begin(), Pattern[i][j].end());
			if (Pattern[i].size() - j != 1)
			{
				Filter.push_back(L';');
			}
		}
		Filter.push_back(L'\0');
	}
	Filter.push_back(L'\0');
	return Filter;
}

std::wstring FTAFileSystemHelpers::GetDocumentsPath()
{
	WCHAR my_documents[MAX_PATH];
	/*HRESULT result = SHGetFolderPath(nullptr, CSIDL_PERSONAL, nullptr, SHGFP_TYPE_CURRENT, my_documents);
	if (result != S_OK)
	{
		return L"";
	}*/
	std::wstring DocPath = my_documents;
	DocPath += L"\\TryAlgebra\\";
	return DocPath;
}

bool FTAFileSystemHelpers::WriteToFile(std::wstring& FileName, const std::wstring& Extension, const std::wstring& FileContents)
{
	namespace fs = std::filesystem;
	fs::path path = FileName;
	// Check if the folder already exists
	if (!fs::exists(path.parent_path()))
	{
		// Attempt to create the directory
		if (!fs::create_directory(path.parent_path()))
		{
			return false;
		}
	}
	//Add extension
	CommonHelpers::RemoveSubstr(FileName, Extension);
	FileName += Extension;
	//Write to file
	std::wofstream f(FileName);
	if (f.is_open())
	{
		f.imbue(std::locale("en_US.UTF-8"));
		f << FileContents;
		f.close();
		return true;
	}
	return false;
}

bool FTAFileSystemHelpers::ReadFromFile(const std::wstring& InPath, std::wstring& OutFile)
{
	std::wifstream wif(InPath);
	if (wif.is_open())
	{
		wif.imbue(std::locale("en_US.UTF-8"));
		std::wstringstream wss;
		wss << wif.rdbuf();
		wif.close();
		OutFile = wss.str();
		return true;
	}
	return false;
}

std::wstring FTAFileSystemHelpers::ToExtension(EDocumentType Type)
{
	switch (Type)
	{
	case EDocumentType::Document:
		return DOCUMENTS_EXT;
	case EDocumentType::Expressions:
		return EXPRESSIONS_EXT;
	case EDocumentType::Script:
		return SCRIPTS_EXT;
	default: ;
	}
	return L"";
}

EDocumentType FTAFileSystemHelpers::ToDocumentType(const std::wstring& Extension)
{
	if (Extension == DOCUMENTS_EXT)
	{
		return EDocumentType::Document;
	}
	if (Extension == EXPRESSIONS_EXT)
	{
		return EDocumentType::Expressions;
	}
	if (Extension == SCRIPTS_EXT)
	{
		return EDocumentType::Script;
	}
	return EDocumentType::Undefined;
}

bool FTAFileSystemHelpers::OpenSimpleDocument(const std::wstring& InPath, FTAMathDocumentInfo& DocumentInfo, EDocumentType Type, const std::shared_ptr<FTACompatibilityData>& CompatibilityData)
{
	std::wstring Data;
	//Try read document data
	if (ReadFromFile(InPath, Data))
	{
		//Create math document
		DocumentInfo.MathDocument = FTAMathDocument::MakeTypedShared();
		//Set Compatibility data
		DocumentInfo.MathDocument->Setup(CompatibilityData);
		//Set math data to document
		DocumentInfo.MathDocument->SetMeData(Data);
		DocumentInfo.FilePath = InPath;
		DocumentInfo.FileName = std::filesystem::path(InPath).filename().wstring();
		DocumentInfo.Type = Type;
		return true;
	}
	return false;
}

std::vector<std::wstring> FTAFileSystemHelpers::GetAllFilesInDirectory(const std::wstring& InDirectory, const std::wstring& InExt)
{
	namespace fs = std::filesystem;
	fs::path Root = InDirectory;
	if (!exists(Root))
	{
		return std::vector<std::wstring>();
	}
	std::vector<std::wstring> result;

	for (const auto& entry : fs::recursive_directory_iterator(Root))
	{
		if (entry.is_regular_file() && entry.path().extension() == InExt)
		{
			result.push_back(entry.path().wstring());
		}
	}

	return result;
}

std::wstring& FTAFileSystemHelpers::MakeRelativeTo(std::wstring& Path, const std::wstring& RootDir)
{
	CommonHelpers::RemoveSubstr(Path, RootDir);
	return Path;
}
