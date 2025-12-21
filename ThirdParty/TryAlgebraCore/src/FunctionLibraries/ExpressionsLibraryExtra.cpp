// Fill out your copyright notice in the Description page of Project Settings.


#include "FunctionLibraries/ExpressionsLibraryExtra.h"

#include "FunctionLibraries/MathElementsHelpers.h"
#include "FunctionLibraries/MeDefinition.h"

std::vector<std::shared_ptr<TAExpressionsLibrary::FTADebugSymbol>> TAExpressionsLibrary::MathTreeToDebug(const FSymbols& MathTree)
{

	std::vector<std::shared_ptr<FTADebugSymbol>> Result;
	Result.reserve(MathTree.size());
	//Iterate over From symbols
	for (auto& Symbol : MathTree)
	{
		//Create new symbol in Result
		Result.push_back(std::make_shared<FTADebugSymbol>());
		auto& DebugSymbol = Result.back();
		DebugSymbol->Info = Symbol->Info;
	}
	for (size_t i = 0; i < Result.size(); i++)
	{
		Result[i]->Children = MathTreeToDebug(MathTree[i]->Children);
	}
	return Result;
}

void TAExpressionsLibrary::DebugMathTree(const FSymbols& MathTree)
{
	/*TArray<FMathDebugNode> Result;
	float Offset = 0;
	auto DebugTree = MathTreeToDebug(MathTree);
	SetupTreePositions(DebugTree,Offset,0);
	CollectDebugData(DebugTree,Result);
	MathElementsHelpers::ShowGraph(Result);*/
}

void TAExpressionsLibrary::SetupTreePositions(const std::vector<std::shared_ptr<FTADebugSymbol>>& DebugTree, float& LastOffset, int Depth)
{
	/*for (auto& Child : DebugTree)
	{
		if (Child->Children.empty())
		{
			Child->Location.X = LastOffset;
			Child->Location.Y = Depth * 100;
			LastOffset += 100;
		}
		else
		{
			SetupTreePositions(Child->Children, LastOffset, Depth + 1);
		}
	}
	
	for (auto& Child : DebugTree)
	{
		if (!Child->Children.empty())
		{
			Child->Location.X = MeanChildrenX(Child->Children);
			Child->Location.Y = Depth * 100;
		}
	}*/
}


float TAExpressionsLibrary::MeanChildrenX(const std::vector<std::shared_ptr<FTADebugSymbol>>& DebugTree)
{
	float Mean = 0;
	/*for (auto& Child : DebugTree)
	{
		Mean += Child->Location.X;
	}*/
	return Mean / DebugTree.size();
}

//void TAExpressionsLibrary::CollectDebugData(const std::vector<std::shared_ptr<FTADebugSymbol>>& DebugTree, TArray<FMathDebugNode>& DebugData)
//{
//	for (auto& Child : DebugTree)
//	{
//		auto& NewEl = DebugData.Emplace_GetRef();
//		NewEl.Info = Child->Info.c_str();
//		NewEl.Location = Child->Location;
//		for (auto& Child2 : Child->Children)
//		{
//			NewEl.ChildrenLocations.Add(Child2->Location);
//		}
//		CollectDebugData(Child->Children, DebugData);
//	}
//	
//}

bool TAExpressionsLibrary::ParseExpressionsFile(const std::wstring& FileContent, const std::vector<FRewritingRules>& RewritingRulesArr, FTAExpressionsFile& OutExpressionsFile)
{
	//Parse file content into math tree
	FSymbols MathTree = ParseToMathTree(FileContent);
	std::vector<FTAExpression> Result;
	enum class EState
	{
		None,
		Name,
		Expression
	} State = EState::None;

	//Iterate over Math tree top level
	for (int i = 0; i < MathTree.size(); i++)
	{
		if (State == EState::None && IsExpressionStart(MathTree[i]))
		{
			//If state = none and Expression start
			//Prepare to read expr name
			Result.emplace_back();
			State = EState::Name;
			continue;
		}
		if (State == EState::Name && (IsWordWrap(MathTree[i]) || IsCharacter(MathTree[i], L":")))
		{
			//If state = Name and symbol = "\n" -> end reading name
			if (Result.back().Name.empty())
			{
				//Expr name is empty
				return false;
			}
			//Start reading Expression
			Result.back().Forms.emplace_back();
			State = EState::Expression;
			continue;
		}
		if (State == EState::Expression && IsCharacter(MathTree[i], L"="))
		{
			//If state = Expr and Symbol = "=" -> next form
			if (Result.back().Forms.empty())
			{
				//if last form was empty
				return false;
			}
			//Start reading new form
			Result.back().Forms.emplace_back();
			continue;
		}
		if (State == EState::Expression && IsExpressionEnd(MathTree[i]))
		{
			State = EState::None;
		}
		if (State == EState::Name)
		{
			//Reading expression name
			Result.back().Name += MathTree[i]->Info;
			continue;
		}
		if (State == EState::Expression)
		{
			//If it is space or wrap-> skip
			if (IsCharacterRange(MathTree[i], L"\n", L" "))
			{
				continue;
			}
			//Reading form
			Result.back().Forms.back().push_back(MathTree[i]);
		}
	}
	//Iterate over expressions
	for (FTAExpression& Expression : Result)
	{
		Expression.Parameters.clear();
		//iterate over forms
		for (FSymbols& Form : Expression.Forms)
		{
			CacheParameters(Form);
			LogicalParse(Form,RewritingRulesArr);			
			ReplaceParamCopies(Form,Expression.Parameters);			
			MarkNodesWNonCommuteSuccessor(Form);
			ExpandNonCommutative(Form);
			MarkNodesWParamSuccessor(Form);
			SortCommutativeNodes(Form);
		}
	}
	OutExpressionsFile.Expressions = Result;
	return true;
}

bool TAExpressionsLibrary::ParseBindings(const std::wstring& MathData, std::map<std::wstring, std::wstring>& OutBindings)
{
	std::map<std::wstring, std::wstring> Result;
	//Parse to math tree
	FSymbols MathTree = ParseToMathTree(MathData);
	enum class EState
	{
		ReadingChord,
		ReadingMathSymbols
	} State = EState::ReadingChord;

	FSymbols CurrentSymbols;
	std::wstring Cord;
	//Iterate over tree
	for (auto& Symbol : MathTree)
	{
		if (State == EState::ReadingChord)
		{
			//If reading chord
			if (!IsCharacter(Symbol))
			{
				return false;
			}
			if (Cord.empty() && IsCharacter(Symbol, L"\n"))
			{
				//Skip empty lines
				continue;
			}
			if (IsCharacter(Symbol, L":"))
			{
				//If meet ":" -> Start reading symbols
				State = EState::ReadingMathSymbols;
				continue;
			}
			//Add to chord
			Cord += Symbol->Info;
		}
		if (State == EState::ReadingMathSymbols)
		{
			//If reading symbols
			if (IsCharacter(Symbol, L"\n"))
			{
				//If meet word wrap -> End binding
				if (Cord.empty() || CurrentSymbols.empty())
				{
					return false;
				}
				Result.emplace(Cord.c_str(), ParseToString(CurrentSymbols));
				Cord.clear();
				CurrentSymbols.clear();
				State = EState::ReadingChord;
				continue;
			}
			CurrentSymbols.push_back(Symbol);
		}
	}
	//Handle last entry
	if (!Cord.empty() && !CurrentSymbols.empty())
	{
		Result.emplace(Cord.c_str(), ParseToString(CurrentSymbols));
	}
	

	OutBindings = Result;
	return true;
}

bool TAExpressionsLibrary::IsExpressionStart(const FSymbolPtr& Symbol)
{
	return Symbol->ElementName == FTAMeDefinition::SpecSymbolName && Symbol->Info == FTAMeDefinition::ExprStartInfo;
}

bool TAExpressionsLibrary::IsExpressionEnd(const FSymbolPtr& Symbol)
{
	return Symbol->ElementName == FTAMeDefinition::SpecSymbolName && Symbol->Info == FTAMeDefinition::ExprEndInfo;
}

