// Fill out your copyright notice in the Description page of Project Settings.


#include "Modules/MathElementsV2/Me/MeMatrix.h"

#include <string>

#include "FunctionLibraries/MeDefinition.h"
#include "FunctionLibraries/MathElementsHelpers.h"
#include "Modules/MathElementsV2/Me/MeContainer.h"


MathElementV2::FTAMeMatrix::FTAMeMatrix(const std::wstring& Info)
{
	FTAMeDefinition::GetMatSize(Info, Rows, Columns);
	SetAdjustChildrenSize(true);
}


std::wstring MathElementV2::FTAMeMatrix::GetParseInfo()
{
	std::wstring Slash = L"\\";
	return Slash + GetName() + Slash + FTAMeDefinition::GetMatInfo(Rows, Columns) + Slash;
}

void MathElementV2::FTAMeMatrix::ArrangeChildren()
{
	int i = -1;
	float YPos = 0;
	float Offset = GetReferenceScaler() / 5.f;
	for (int x = 0; x < Rows; x++)
	{
		float XPos = 0;
		for (int y = 0; y < Columns; y++)
		{
			i++;
			Children[i]->SetLocalPosition({ XPos, YPos });
			XPos = Children[i]->GetLocalPosition().x + Children[i]->GetAbsoluteSize().x + Offset;
		}
		XPos = 0;
		YPos = Children[i]->GetLocalPosition().y + Children[i]->GetAbsoluteSize().y + Offset;
	}
}

void MathElementV2::FTAMeMatrix::AdjustChildrenSize()
{
	std::vector<float> MaxHeights;
	MaxHeights.resize(Rows,0.f);
	std::vector<float> MaxWidths;
	MaxWidths.resize(Columns, 0.f);
	
	int i = -1;
	for (int Y = 0; Y < Rows; Y++)
	{
		for (int X = 0; X < Columns; X++)
		{
			i++;
			MaxHeights[Y] = (std::max)(Children[i]->GetAbsoluteSize().y, MaxHeights[Y]);
			MaxWidths[X] = (std::max)(Children[i]->GetAbsoluteSize().x, MaxWidths[X]);
		}
	}
	i = -1;
	for (int Y = 0; Y < Rows; Y++)
	{
		int ArrangeFrom = i + 1;
		for (int X = 0; X < Columns; X++)
		{
			i++;
			TACommonTypes::FTAVector2d NewSize = {MaxWidths[X], MaxHeights[Y]};
			TACommonTypes::FTAVector2d HalfDifference = (NewSize - Children[i]->GetAbsoluteSize());
			if (bCenterContent)
			{
				HalfDifference.x /= 2;
			}
			else
			{
				HalfDifference.x = 0;
			}
			Children[i]->SetAbsoluteSize(NewSize);
			if (auto Cont = Children[i]->Cast<FTAMeContainer>())
			{
				FTAMeHelpers::OffsetMathElements(Cont->GetChildren(), HalfDifference);
			}
		}
		int ArrangeTo = i + 1;
		FTAMeHelpers::AlignContentInContainersVertically(Children, ArrangeFrom, ArrangeTo);
	}
}

std::wstring MathElementV2::FTAMeMatrix::GetName()
{
	return FTAMeDefinition::MatrixName;
}

void MathElementV2::FTAMeMatrix::GetSize(int& OutRows, int& OutColumns)
{
	OutRows = Rows;
	OutColumns = Columns;
}
