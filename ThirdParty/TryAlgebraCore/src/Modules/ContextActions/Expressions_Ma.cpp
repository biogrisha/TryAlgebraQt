// Fill out your copyright notice in the Description page of Project Settings.

#include <algorithm>
#include <vector>
#include "Modules/ContextActions/Expressions_Ma.h"
#include "FunctionLibraries/CommonHelpers.h"
#include "Modules/Expressions/CustomTokenization.h"
#include "Modules/Expressions/ExpressionsComponent.h"
#include "Modules/Expressions/ExpressionsModule.h"
#include "Modules/MathDocument/MathDocument.h"


FTAExpressions_Ma::FTAExpressions_Ma(const FDependency& InDependency)
{
	ActionName = L"Expressions";
	Dependency = InDependency;
}

bool FTAExpressions_Ma::CanPerformAction(const TTypedWeak<class FTAMathDocument>& InDocument)
{
	Document = InDocument;
	//Clear cached expressions
	ResultingExpressions.clear();
	//Get selected elements
	std::wstring MathData =	Document->GetSelectedMeData();
	//Parse to math tree
	TAExpressionsLibrary::FSymbols MathTree = TAExpressionsLibrary::ParseToMathTree(MathData);
	//Remove " " and "\n"
	TAExpressionsLibrary::LogicalParse(MathTree,Dependency.CustomTokenization.lock()->GetRewritingRulesArr());
	//--------------Test----------
	/*ResultingExpressions.emplace_back();
	ResultingExpressions.back().FileName = L"Test";
	ResultingExpressions.back().Expressions.emplace_back();
	ResultingExpressions.back().Expressions.back().Name = L"Test";
	auto& RewritingRules = Dependency.CustomTokenization.lock()->GetRewritingRulesArr();
	TAExpressionsLibrary::LogicalParse(MathTree, RewritingRules);
	TAExpressionsLibrary::SimplifyMathTree(MathTree);
	ResultingExpressions.back().Expressions.back().Forms.push_back(TAExpressionsLibrary::ParseToString(MathTree));*/
	//----------------------------
	
	//Get expression module expr-files and current doc expr-file
	auto ExprFiles = Dependency.ExpressionsModule.lock()->GetExpressionsFiles();
	if (auto ExprComp = InDocument->GetComponent<FTAExpressionsComponent>())
	{
		ExprFiles.insert(ExprFiles.begin(),ExprComp->GetExpressions());
	}
	//Iterate over files
	for (TAExpressionsLibrary::FTAExpressionsFile& File : ExprFiles)
	{
		FExpressionsFile ExpressionsFile;
		ExpressionsFile.FileName = File.FileName.c_str();
		//Iterate over expressions
		for (TAExpressionsLibrary::FTAExpression& Expression : File.Expressions)
		{
			
			//Iterate over forms
			for (auto& Form : Expression.Forms)
			{
				/*//Try to apply one of the form
				TAExpressionsLibrary::FParametersHelper ParametersHelper;
				if (!TAExpressionsLibrary::ExtractArgumentsOld(MathTree,Form,&ParametersHelper))
				{
					continue;
				}
				//if succeeded
				FExpression ExpressionTemp;
				ExpressionTemp.Name = Expression.Name.c_str();
				//Iterate again over forms
				for (auto& Form1 : Expression.Forms)
				{
					//Copy Form and replace parameters with args
					TAExpressionsLibrary::FSymbols FormCopy = TAExpressionsLibrary::CopyMathTree(Form1);
					//Simplify and parse to string array
					TAExpressionsLibrary::SimplifyMathTree(FormCopy);
					TAExpressionsLibrary::ReplaceParametersWithArguments(&ParametersHelper,FormCopy);
					std::wstring FormMathData = TAExpressionsLibrary::ParseToString(FormCopy);
					//Add into expression
					ExpressionTemp.Forms.push_back(FormMathData);
				}
				//Add Expression into file
				ExpressionsFile.Expressions.push_back(ExpressionTemp);
				//Execution reached here when successfully found desired Form -> break Forms loop
				break;*/

				TAExpressionsLibrary::ClearCapturedArguments(Expression.Parameters);
				TAExpressionsLibrary::FComparatorTest Comparator;
				if (!Comparator.Compare(MathTree, Form))
				{
					continue;
				}
				//if succeeded
				FExpression ExpressionTemp;
				ExpressionTemp.Name = Expression.Name.c_str();
				//TAExpressionsLibrary::RemoveContextOperator(Expression.Parameters);
				//Iterate again over forms
				for (auto& FormPure : Expression.Forms)
				{
					TAExpressionsLibrary::FSymbols FormCopy = TAExpressionsLibrary::CopyMathTree(FormPure);
					TAExpressionsLibrary::ReplaceParametersWithArguments(FormCopy);
					TAExpressionsLibrary::RemoveSurplusOperators(FormCopy);
					TAExpressionsLibrary::SimplifyMathTree(FormCopy);
					std::wstring FormMathData = TAExpressionsLibrary::ParseToString(FormCopy);
					//Add into expression
					ExpressionTemp.Forms.push_back(FormMathData);
				}
				//Add Expression into file
				ExpressionsFile.Expressions.push_back(ExpressionTemp);
				//Execution reached here when successfully found desired Form -> break Forms loop
				break;
				
			}
		}
		if (!ExpressionsFile.Expressions.empty())
		{
			//If found suitable expressions -> add file to result
			ResultingExpressions.push_back(ExpressionsFile);
		}
	}
	
	return !ResultingExpressions.empty();
}

const std::vector<FTAExpressions_Ma::FExpressionsFile>& FTAExpressions_Ma::GetExpressionsFiles() const
{
	return ResultingExpressions;
}

void FTAExpressions_Ma::SetCurrentExpression(int InFileIndex, int InExpressionIndex)
{
	FileIndex = InFileIndex;
	ExpressionIndex = InExpressionIndex;
	FormIndex = 0;
	InsertForm();
}

void FTAExpressions_Ma::NextForm(int Delta)
{
	FormIndex += Delta;
	FormIndex = (std::clamp)(FormIndex, 0, GetCurrentFormsNum()-1);
	InsertForm();
}

int FTAExpressions_Ma::GetCurrentFormsNum()
{
	if (CommonHelpers::IsValidId(ResultingExpressions,FileIndex))
	{
		if (CommonHelpers::IsValidId(ResultingExpressions[FileIndex].Expressions,ExpressionIndex))
		{
			return ResultingExpressions[FileIndex].Expressions[ExpressionIndex].Forms.size();
		}
	}
	return 0;
}

void FTAExpressions_Ma::InsertForm()
{
	if (CommonHelpers::IsValidId(ResultingExpressions,FileIndex))
	{
		if (CommonHelpers::IsValidId(ResultingExpressions[FileIndex].Expressions,ExpressionIndex))
		{
			Document->AddSelectMathElements(ResultingExpressions[FileIndex].Expressions[ExpressionIndex].Forms[FormIndex]);
		}
	}
}
