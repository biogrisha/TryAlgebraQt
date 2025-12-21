// Fill out your copyright notice in the Description page of Project Settings.


#include "Modules/ContextActions/Matrix_Ma.h"

#include "FunctionLibraries/MeDefinition.h"
#include "Modules/CursorComponent/CursorComponent.h"
#include "Modules/MathDocument/MathDocument.h"
#include "Modules/MathElementsV2/Me/MeCases.h"
#include "Modules/MathElementsV2/Me/MeContainer.h"
#include "Modules/MathElementsV2/Me/MeMatrix.h"

FTAMatrix_Ma::FTAMatrix_Ma()
{
	ActionName = L"Matrix";
	Rows = 0;
	Columns = 0;
}

bool FTAMatrix_Ma::CanPerformAction(const TTypedWeak<class FTAMathDocument>& InDocument)
{
	auto& SelectedElements = InDocument->GetCursorComponent()->GetSelectedElements();
	if (SelectedElements.size() == 1 && SelectedElements[0]->IsOfType(MathElementV2::FTAMeMatrix::StaticType()))
	{
		Document = InDocument;
		auto MeMat = SelectedElements[0]->Cast<MathElementV2::FTAMeMatrix>();
		MeMat->GetSize(Rows, Columns);
		if (auto MeCases = SelectedElements[0]->Cast<MathElementV2::FTAMeCases>())
		{
			Bracket = std::wstring(1, MeCases->GetBracket());
		}
		MathTree = TAExpressionsLibrary::ParseToMathTree(Document->GetSelectedMeData());
		return true;
	}
	return false;
}

void FTAMatrix_Ma::OnCantPerform()
{
	MathTree.clear();
	Bracket.clear();
}

void FTAMatrix_Ma::ChangeMatrixSize(int XSize, int YSize)
{
	auto MathTreeCopy = TAExpressionsLibrary::CopyMathTree(MathTree);
	auto& Containers = MathTreeCopy[0]->Children;
	Containers.clear();
	for (int R = 0; R < YSize; R++)
	{
		for (int C = 0; C < XSize; C++)
		{
			if (C < Columns && R < Rows)
			{
				Containers.push_back(GetMatContainer(C,R));
			}
			else
			{
				Containers.push_back(TAExpressionsLibrary::MakeSymbol(MathElementV2::FTAMeContainer::GetName(), L""));
			}
		}
	}
	if (!Bracket.empty())
	{
		MathTreeCopy[0]->Info = FTAMeDefinition::GetCasesInfo(YSize, XSize, Bracket);
	}
	else
	{
		MathTreeCopy[0]->Info = FTAMeDefinition::GetMatInfo(YSize, XSize);
	}
	std::wstring Result = TAExpressionsLibrary::ParseToString(MathTreeCopy);
	Document->AddSelectMathElements(Result);
}

void FTAMatrix_Ma::GetMatrixSize(int& XSize, int& YSize)
{
	XSize = Columns;
	YSize = Rows;
}

TAExpressionsLibrary::FSymbolPtr FTAMatrix_Ma::GetMatContainer(int C, int R)
{
	int Id = C + R * Columns;
	return TAExpressionsLibrary::CopyMathTree({MathTree[0]->Children[Id]})[0];
}
