// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include <string>

class FTAMeDefinition
{
public:
	static inline std::wstring FromToName = L"Ft";
	static inline std::wstring IntegralInfo = L"∫";
	static inline std::wstring DoubleIntegralInfo = L"∬";
	static inline std::wstring TripleIntegralInfo = L"∭";
	static inline std::wstring QuadrupleIntegralInfo = L"⨌";
	static inline std::wstring ContourIntegralInfo = L"∮";
	static inline std::wstring SummationInfo = L"∑";
	static inline std::wstring ProductInfo = L"Π";
	static inline std::wstring UnionOverInfo = L"∪";
	
	
	static inline std::wstring IndexName = L"Ind";
	static inline std::wstring SubscriptInfo = L"SbScr";
	static inline std::wstring SuperscriptInfo = L"SpScr";
	static inline std::wstring DoublescriptInfo = L"DScr";
	
	static inline std::wstring MatrixName = L"Mat";
	
	static inline std::wstring HatName = L"Hat";
	static inline std::wstring HatInfo = L"Hat";
	static inline std::wstring HatVecInfo = L"Vec";
	static inline std::wstring HatBarInfo = L"Bar";
	
	static inline std::wstring FractionName = L"Frc";
	
	static inline std::wstring SpecSymbolName = L"Ss";
	static inline std::wstring CursorPlacementInfo = L"Cur";
	static inline std::wstring EmbraceSymbolInfo = L"Emb";
	static inline std::wstring ExprStartInfo = L"EStart";
	static inline std::wstring ExprEndInfo = L"EEnd";
	
	static inline std::wstring ContainerName = L"Cont";
	
	static inline std::wstring ParamName = L"Par";
	static inline std::wstring SingleParamInfo = L"Sing";
	static inline std::wstring MultiParamInfo = L"Mlt";

	static inline std::wstring CustomTokenName = L"CTok";
	static inline std::wstring NonCommuteInfo = L"NCom";
	static inline std::wstring TokenWord = L"TWord";
	static inline std::wstring TokenExpression = L"TExpr";

	static inline std::wstring CasesName = L"Cas";
	static inline std::wstring CasesCurly = L"{";
	static inline std::wstring CasesSquare = L"[";
	static std::wstring GetCasesInfo(int Rows, int Cols, const std::wstring& Bracket);

	static inline std::wstring FunctionName = L"Func";
	
	static inline std::wstring TextBlockName = L"Text";
	
	static inline std::wstring InvisibleBracketsType = L"IB";
	static inline std::wstring OperatorType = L"OP";
	static inline std::wstring MeInvalidType = L"I";
	
	static std::wstring GetMatInfo(int Rows, int Cols);
	static void GetMatSize(const std::wstring& MatInfo, int& Rows, int& Cols);
	
};
