// Fill out your copyright notice in the Description page of Project Settings.


#include "FunctionLibraries/ScriptingHelpers.h"

#include "GlobalVariables.h"
#include "FunctionLibraries/FileHelpers.h"

//TArray<FString> ScriptingHelpers::VectorFromRaw(wchar_t** StrArray, int InStringsNum)
//{
//	//Create vector of str with size of input array
//	TArray<FString> InMathElementsData;
//	InMathElementsData.SetNum(InStringsNum);
//	//copy all strings in vector
//	for (int i = 0; i < InStringsNum; i++)
//	{
//		InMathElementsData[i] = StrArray[i];
//	}
//	return InMathElementsData;
//}
//
//const wchar_t** ScriptingHelpers::RawFromVector(const TArray<FString>& InMathElementsData, int& Num)
//{
//	if (InMathElementsData.IsEmpty())
//	{
//		return nullptr;
//	}
//	//create raw wchar_t* array
//	auto OutStrings = new const wchar_t*[InMathElementsData.Num()];
//	//specify output size of it
//	Num = InMathElementsData.Num();
//	for (int i = 0; i < Num; i++)
//	{
//		//Create array of content size + 1
//		OutStrings[i] = *InMathElementsData[i];
//	}
//	return OutStrings;
//}
//
//void ScriptingHelpers::MathElementsToCode(const TArray<ParseHelpers::FMathEl>& InMathElementsData, FString& Code)
//{
//	Code+= "{";
//	for(int32 i = 0; i < InMathElementsData.Num(); i++)
//	{
//		Code+= "MeInnerHelpers::MakeSymbol(L\"" + InMathElementsData[i]->ElementName + "\",L\""+ InMathElementsData[i]->Info + "\",nullptr";
//		if(!InMathElementsData[i]->Children.IsEmpty())
//		{
//			Code+= ",";
//			MathElementsToCode(InMathElementsData[i]->Children, Code);
//		}
//		Code+= ")";
//		if(i < InMathElementsData.Num() - 1)
//		{
//			Code+= ",";
//		}
//	}
//	Code+= "}";
//}
//
//FString ScriptingHelpers::ParseToScript(const TArray<FString>& MathElementsData)
//{
//	//PArse to math tree
//	TArray<ParseHelpers::FMathEl> MathTree = ParseHelpers::ParseToMathTree(MathElementsData);
//	//Replace math section with branch
//	ParseHelpers::MergeSymbols(MathTree,"#","(");
//	ParseHelpers::MergeSymbols(MathTree,")","#");
//	ParseHelpers::SetInvisibleBracketsAroundBrackets(MathTree,"#(",")#");
//	ParseHelpers::InvisibleBracketsToBranches(MathTree);
//	ParseHelpers::RemoveSymbol(MathTree,"#(");
//	ParseHelpers::RemoveSymbol(MathTree,")#");
//	FString Code;
//	//Iterate over top level of the tree
//	for(int32 i = 0; i < MathTree.Num(); i++)
//	{
//		if(ParseHelpers::IsSymbol(MathTree[i]))
//		{
//			//Parse symbols into text
//			Code+=ParseHelpers::SymbolToString(MathTree[i]);
//		}
//		else if(ParseHelpers::IsInvisibleBrackets(MathTree[i]))
//		{
//			//Parse math branch into math tree structure 
//			Code+= TEXT("MeHelpers::SetParents(");
//			ScriptingHelpers::MathElementsToCode(MathTree[i]->Children,Code);
//			Code+= TEXT(")");
//		}
//	}
//	return Code;
//}
//
//FString ScriptingHelpers::CompileLibrary(const FString& CodeSrc, const FString& CodeHdr, const FString& PathSrc, const FString& PathHdr, const FString& LibName)
//{
//	FFileHelper::SaveStringToFile(CodeSrc, *(FTAFileSystemHelpers::GetDocumentsPath_Old() + SCRIPTS_PATH +PathSrc));
//	FFileHelper::SaveStringToFile(CodeHdr, *(FTAFileSystemHelpers::GetDocumentsPath_Old() + SCRIPTS_PATH +PathHdr));
//	FString CmdConfigPath = FTAFileSystemHelpers::FindFirstFileFolder("C:\\Program Files\\Microsoft Visual Studio", "vcvars64.bat");
//	FString Command = "cd " + CmdConfigPath;
//	Command += " & vcvars64.bat";
//	Command += " & cd \"" + FTAFileSystemHelpers::GetDocumentsPath_Old() + SCRIPTS_PATH + "\"";
//	Command += " & cl /LD " + PathSrc + " /Fe:" + LibName;
//	return FTAFileSystemHelpers::ExecCommand(*Command);
//}
