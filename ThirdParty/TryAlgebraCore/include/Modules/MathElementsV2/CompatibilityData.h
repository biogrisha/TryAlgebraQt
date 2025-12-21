// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Me/MathElementBaseV2.h"

class FTAMathElementGenerator
{
public:
	virtual ~FTAMathElementGenerator() = default;
	virtual MathElementV2::FMathElementPtr CreateMathElement(const std::wstring& Name,const std::wstring& Info) = 0;
	virtual MathElementV2::FMathElementPtr CreateCharacter(wchar_t Char) = 0;
};
class FTACursorComponentGenerator
{
public:
	virtual ~FTACursorComponentGenerator() = default;
	virtual std::shared_ptr<class FTACursorComponent> Create() = 0;
};
class FTACompatibilityData : public FTATypedObject
{
	TYPED_CLASS_META(FTACompatibilityData,FTATypedObject)
public:
	std::shared_ptr<FTAMathElementGenerator> MeGenerator;
	std::shared_ptr<FTACursorComponentGenerator> CursorComponentGenerator;
};
