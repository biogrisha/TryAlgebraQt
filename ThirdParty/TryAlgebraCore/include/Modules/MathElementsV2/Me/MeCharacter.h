// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "MathElementBaseV2.h"
#include "Modules/CommonTypes/Color.h"



namespace MathElementV2
{
	class FTAMeCharacter : public FTAMathElementBase
	{
		TYPED_CLASS_META(FTAMeCharacter,FTAMathElementBase)
	protected:
		FTAMeCharacter(wchar_t InChar);
		virtual std::wstring GetParseInfo() override;
	public:
		wchar_t GetChar() const { return Char; };
		TACommonTypes::FTAColor& GetColor() { return Color; }
		void SetColor(const TACommonTypes::FTAColor& InColor) { Color = InColor; }
	protected:
		wchar_t Char;
		TACommonTypes::FTAColor Color;
	};
}
