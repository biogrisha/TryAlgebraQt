// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "BaseMathAction.h"
#include <vector>
class FTAExpressions_Ma : public FTABaseMathAction
{
	TYPED_CLASS_META(FTAExpressions_Ma, FTABaseMathAction)
public:
	struct FDependency
	{
		std::weak_ptr<class FTAExpressionsModule> ExpressionsModule;
		std::weak_ptr<class FTACustomTokenization> CustomTokenization;
	};
	struct FExpression
	{
		std::wstring Name;
		std::vector<std::wstring> Forms;
	};

	struct FExpressionsFile
	{
		std::wstring FileName;
		std::vector<FExpression> Expressions;
	};
protected:
	FTAExpressions_Ma(const FDependency& InDependency);
public:
	virtual bool CanPerformAction(const TTypedWeak<class FTAMathDocument>& InDocument) override;
	const std::vector<FExpressionsFile>& GetExpressionsFiles() const;
	void SetCurrentExpression(int InFileIndex, int InExpressionIndex);
	void NextForm(int Delta);
private:
	int GetCurrentFormsNum();
	void InsertForm();
	FDependency Dependency;
	TTypedWeak<class FTAMathDocument> Document;
	std::vector<FExpressionsFile> ResultingExpressions;
	int FileIndex = 0;
	int ExpressionIndex = 0;
	int FormIndex = 0;
};
