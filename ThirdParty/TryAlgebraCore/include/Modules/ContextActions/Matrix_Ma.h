// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "BaseMathAction.h"
#include "FunctionLibraries/ExpressionsLibrary.h"


class FTAMatrix_Ma : public FTABaseMathAction
{
	TYPED_CLASS_META(FTAMatrix_Ma,FTABaseMathAction)
public:
	FTAMatrix_Ma();
	virtual bool CanPerformAction(const TTypedWeak<class FTAMathDocument>& InDocument) override;
	virtual void OnCantPerform() override;
	void ChangeMatrixSize(int XSize, int YSize);
	void GetMatrixSize(int& XSize, int& YSize);
private:
	TAExpressionsLibrary::FSymbolPtr GetMatContainer(int C, int R);
	TAExpressionsLibrary::FSymbols MathTree;
	int Rows;
	int Columns;
	std::wstring Bracket;
	TTypedWeak<class FTAMathDocument> Document;
};
