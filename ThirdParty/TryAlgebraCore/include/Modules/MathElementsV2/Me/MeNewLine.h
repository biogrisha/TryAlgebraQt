// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "MeCharacter.h"
struct FTAMeHelpers;
namespace MathElementV2
{
	class FTAMeNewLine : public FTAMeCharacter
	{
		TYPED_CLASS_META(FTAMeNewLine, FTAMeCharacter)
		friend FTAMeHelpers;
	protected:
		FTAMeNewLine();
	public:
		int GetElementsCount() const { return ElementsCount; }
	private:
		//Number of elements in line, excluding NewLine
		int ElementsCount = 0;
	public:
		static std::wstring GetName() { return L"lb"; }

		//new logic
	virtual void CalculateSize(float AccumulatedScalingFactor) override;
	};
}
