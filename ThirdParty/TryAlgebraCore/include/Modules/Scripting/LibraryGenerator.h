// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Windows.h"
#include "Modules/ContextActions/Expressions_Ma.h"

//class FScriptsLibrary
//{
//	using f_TryApplyExpressions = void(__stdcall*)(const wchar_t** InStrings, int InStringsNum, wchar_t**& OutStrings, int& OutStringsNum);
//	using f_ClearExpressionsData = void(__stdcall*)(wchar_t**& Strings, int StringsNum);
//	using f_TestFunc = int(__stdcall*)();
//	public:
//	FScriptsLibrary(const FString& LibName);
//	~FScriptsLibrary();
//	void TryApplyExpressions(const wchar_t** InStrings, int InStringsNum, wchar_t**& OutStrings, int& OutStringsNum);
//	void ClearExpressionsData(wchar_t**& Strings, int StringsNum);
//private:
//	f_TestFunc TestFunc;
//	HINSTANCE DllHandle;
//};
//class FDynamicScriptsComponent
//{
//public:
//	void LoadScripts();
//	TArray<FTAExpressions_Ma::FExpressionsFile> TryApplyScripts(const TArray<FString>& InMathData);
//private:
//	void AddScript(const FString& Name,const TArray<FString>& MathElementsData);
//	TArray<FTAExpressions_Ma::FExpressionsFile> SeparateMathData(const TArray<FString>& ScriptsResult);
//	struct FScriptsFile
//	{
//		FString Name;
//		TMap<FString,FString> Scripts;
//	};
//	TArray<FScriptsFile> ScriptFiles;
//	TSharedPtr<FScriptsLibrary> ScriptsLibrary;
//};
