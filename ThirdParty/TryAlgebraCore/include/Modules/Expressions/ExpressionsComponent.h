// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "FunctionLibraries/ExpressionsLibraryExtra.h"
#include "Modules/ComponentBase.h"

class FTAExpressionsComponent : public FTAComponentBase
{
	TYPED_CLASS_META(FTAExpressionsComponent,FTAComponentBase);
protected:
	FTAExpressionsComponent(const std::wstring& InFilename, const std::weak_ptr<class FTACustomTokenization>& InCustomTokenization);
public:
	void LoadExpressions();
	const TAExpressionsLibrary::FTAExpressionsFile& GetExpressions() const;
private:
	TAExpressionsLibrary::FTAExpressionsFile Expressions;
	std::wstring Filename;
	std::weak_ptr<class FTACustomTokenization> CustomTokenization;
};
