// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "MathElementBaseV2.h"


namespace MathElementV2
{
	class FTAMeSpecialSymbol : public FTAMathElementBase
	{
		TYPED_CLASS_META(FTAMeSpecialSymbol,FTAMathElementBase)
	protected:
		FTAMeSpecialSymbol(const std::wstring& InName);
	public:
		virtual std::wstring GetParseInfo() override;
		const std::wstring& GetSymbolName();
		static std::wstring GetName();
	private:
		std::wstring SymbolName;
	};
}
