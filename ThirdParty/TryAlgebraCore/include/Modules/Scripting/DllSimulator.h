// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

namespace DllSimulator
{
	const wchar_t* GetStartScriptString();
	void TryApplyScripts(const wchar_t** InStrings, int InStringsNum, wchar_t**& OutStrings, int& OutStringsNum);
	void ClearExpressionsData(wchar_t**& Strings, int StringsNum);
}

