// Fill out your copyright notice in the Description page of Project Settings.


#include "Modules/MathDocument/CommandBindings.h"

#include "GlobalVariables.h"
#include "FunctionLibraries/ExpressionsLibraryExtra.h"
#include "FunctionLibraries/FileHelpers.h"
#include "FunctionLibraries/MathElementsHelpers.h"
#include "FunctionLibraries/MeDefinition.h"

void FTACommandBindings::UpdateBindings()
{
	std::wstring FileContent;
	FTAFileSystemHelpers::ReadFromFile(FTAFileSystemHelpers::GetDocumentsPath() + BINDINGS_PATH, FileContent);
	TAExpressionsLibrary::ParseBindings(FileContent, Bindings);
}

void FTACommandBindings::AddToCommand(const std::wstring& Key)
{
	CurrentCommand += Key;
}

std::wstring FTACommandBindings::ApplyCommand(const std::wstring& SelectedMeData, std::vector<int>& OutSubPath)
{
	//Find binding
	auto Res = Bindings.find(CurrentCommand);
	//Clear command
	CurrentCommand.clear();
	if (Res != Bindings.end())
	{
		//if found
		//Parse bound Me to MathTree
		auto ResultMathTree = TAExpressionsLibrary::ParseToMathTree(Res->second);		
		//Replace symbol "Insert selected" with selected data
		TAExpressionsLibrary::FSymbols InsertSelectedMathTree = {TAExpressionsLibrary::MakeSymbol(FTAMeDefinition::SpecSymbolName, FTAMeDefinition::EmbraceSymbolInfo)};
		if (InsertSelectedMathTree.empty())
		{
			return L"";
		}
		TAExpressionsLibrary::ReplaceSymbol(ResultMathTree, InsertSelectedMathTree[0], SelectedMeData);

		//Find Cursor placement path
		TAExpressionsLibrary::FSymbols CursorPlacementMathTree = {TAExpressionsLibrary::MakeSymbol(FTAMeDefinition::SpecSymbolName, FTAMeDefinition::CursorPlacementInfo)};
		TAExpressionsLibrary::FindPathToFirst(ResultMathTree, CursorPlacementMathTree[0], OutSubPath);
		// Remove all occurrence of Cursor placement
		TAExpressionsLibrary::RemoveAllSymbols(ResultMathTree, CursorPlacementMathTree[0]);
		
		//Parse result back to string
		return TAExpressionsLibrary::ParseToString(ResultMathTree);
	}
	return L"";
}
