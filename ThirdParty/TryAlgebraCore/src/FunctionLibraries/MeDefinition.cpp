// Fill out your copyright notice in the Description page of Project Settings.


#include "FunctionLibraries/MeDefinition.h"

#include <string>

#include "FunctionLibraries/CommonHelpers.h"


std::wstring FTAMeDefinition::GetCasesInfo(int Rows, int Cols, const std::wstring& Bracket)
{
	return GetMatInfo(Rows, Cols) + L"," + Bracket;
}

std::wstring FTAMeDefinition::GetMatInfo(int Rows, int Cols)
{
	return std::to_wstring(Rows) + L"," + std::to_wstring(Cols);
}

void FTAMeDefinition::GetMatSize(const std::wstring& MatInfo, int& Rows, int& Cols)
{
	auto Params = CommonHelpers::SplitStr(MatInfo, L',');
	Rows = std::stoi(Params[0]);
	Cols = std::stoi(Params[1]);
}
