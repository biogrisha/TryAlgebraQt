// Fill out your copyright notice in the Description page of Project Settings.


#include "Modules/Scripting/DllSimulator.h"

#include <map>

#include "FunctionLibraries/ExpressionsLibrary.h"

using namespace TAExpressionsLibrary;
namespace DllSimulator
{
	FSymbols Func1(const FSymbols& InputExpression)
	{
		return CopyMathTree(InputExpression);
	}
	
}

const wchar_t* DllSimulator::GetStartScriptString()
{
	return L"~~~";
}

void DllSimulator::TryApplyScripts(const wchar_t** InStrings, int InStringsNum, wchar_t**& OutStrings, int& OutStringsNum)
{
	/*//Create vector of str with size of input array
	std::vector<std::wstring> InMathElementsData(InStringsNum);
	//copy all strings in vector
	for (int i = 0; i < InStringsNum; i++)
	{
		InMathElementsData[i] = InStrings[i];
	}

	const FSymbols Symbols = ParseToMathTree(InMathElementsData);
	std::vector<std::wstring> OutMathElementsData;
	
	OutMathElementsData.push_back(GetStartScriptString());
	OutMathElementsData.push_back(L"Filename");
	OutMathElementsData.push_back(L"Func1Name");
	auto Func1Result = ParseToMathData(Func1(Symbols));
	OutMathElementsData.insert(OutMathElementsData.end(), Func1Result.begin(), Func1Result.end());

	
	//create raw wchar_t* array
	OutStrings = new wchar_t*[OutMathElementsData.size()];
	//specify output size of it
	OutStringsNum = OutMathElementsData.size();
	for (int i = 0; i < OutStringsNum; i++)
	{
		//Create array of content size + 1
		OutStrings[i] = new wchar_t[OutMathElementsData[i].length() + 1];
		wcscpy_s(OutStrings[i], OutMathElementsData[i].length() + 1, OutMathElementsData[i].c_str());
	}*/
}

void DllSimulator::ClearExpressionsData(wchar_t**& Strings, int StringsNum)
{
	for (int i = 0; i < StringsNum; i++)
	{
		delete[] Strings[i];
	}
	delete[] Strings;
	Strings = nullptr;
}

