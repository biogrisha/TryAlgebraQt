// Fill out your copyright notice in the Description page of Project Settings.


#include "Modules/Scripting/LibraryGenerator.h"

#include "GlobalVariables.h"
#include "FunctionLibraries/FileHelpers.h"
#include "FunctionLibraries/ScriptingHelpers.h"
#include "Modules/Scripting/DllSimulator.h"

//
//FScriptsLibrary::FScriptsLibrary(const FString& LibName)
//{
//	/*TestFunc = nullptr;
//	DllHandle = nullptr;
//	FString DllPath = FileSystemHelpers::GetDocumentsPath() + SCRIPTS_PATH + LibName + ".dll";
//	DllHandle = LoadLibrary(*DllPath);
//
//	if (!DllHandle)
//	{
//		return;
//	}
//	TestFunc = reinterpret_cast<f_TestFunc>(GetProcAddress(DllHandle, "HelloWorld"));*/
//}
//
//FScriptsLibrary::~FScriptsLibrary()
//{
//	/*if(DllHandle)
//	{
//		FreeLibrary(DllHandle);
//	}*/
//}
//
//void FScriptsLibrary::TryApplyExpressions(const wchar_t** InStrings, int InStringsNum, wchar_t**& OutStrings, int& OutStringsNum)
//{
//	DllSimulator::TryApplyScripts(InStrings, InStringsNum, OutStrings, OutStringsNum);
//}
//
//void FScriptsLibrary::ClearExpressionsData(wchar_t**& Strings, int StringsNum)
//{
//	DllSimulator::ClearExpressionsData(Strings, StringsNum);
//}
//
//void FDynamicScriptsComponent::LoadScripts()
//{
//	//Find scripts directory
//	FString Dir = FTAFileSystemHelpers::GetDocumentsPath_Old() + SCRIPTS_PATH;
//	//Find all script documents
//	TArray<FString> Files;
//	FString SearchTemplate = "*";
//	SearchTemplate += SCRIPTS_PATH;
//	IFileManager::Get().FindFilesRecursive(Files, *Dir, *SearchTemplate, true, true, false);
//	//Iterate over files
//	for (const FString& File : Files)
//	{
//		TArray<FString> FileContent;
//		FTAFileSystemHelpers::OpenFileAsStringArray(File, FileContent);
//		
//		
//	}
//}
//
//TArray<FTAExpressions_Ma::FExpressionsFile> FDynamicScriptsComponent::TryApplyScripts(const TArray<FString>& InMathData)
//{
//	//Get raw data and data size from MathElementsData
//	int Num;
//	const wchar_t** RawFromVector = ScriptingHelpers::RawFromVector(InMathData, Num);
//	if (!RawFromVector)
//	{
//		return {};
//	}
//	//Resulting string array and size
//	wchar_t** StringsPtr = nullptr;
//	int StringsNum = 0;
//	//Apply dynamic scripts
//	ScriptsLibrary->TryApplyExpressions(RawFromVector,InMathData.Num(),StringsPtr,StringsNum);
//	if (StringsNum == 0)
//	{
//		return {};
//	}
//	//Convert in UE Array
//	TArray<FString> VectorFromRaw = ScriptingHelpers::VectorFromRaw(StringsPtr,StringsNum);
//	//Clear array allocated in dll
//	ScriptsLibrary->ClearExpressionsData(StringsPtr,StringsNum);
//	//Parse in Expressions form
//	return SeparateMathData(VectorFromRaw);
//}
//
//TArray<FTAExpressions_Ma::FExpressionsFile> FDynamicScriptsComponent::SeparateMathData(const TArray<FString>& ScriptsResult)
//{
//	TArray<FTAExpressions_Ma::FExpressionsFile> Result;
//	return Result;
//}