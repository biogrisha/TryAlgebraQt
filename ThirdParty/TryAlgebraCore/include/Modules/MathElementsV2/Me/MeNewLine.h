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
		void SetMinY(float Val);
		int GetElementsCount() const { return ElementsCount; }
	private:
		float Height = 1;
		float MinY = 0;
		int ElementsCount = 0;
	public:
		static std::wstring GetName() { return L"lb"; }
	};
}
