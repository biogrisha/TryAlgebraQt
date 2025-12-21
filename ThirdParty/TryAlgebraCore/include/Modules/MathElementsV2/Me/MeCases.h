// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MeComposite.h"
#include "MeMatrix.h"

namespace MathElementV2
{
	class FTAMeCases : public FTAMeMatrix
	{
		TYPED_CLASS_META(FTAMeCases, FTAMeMatrix)
	protected:
		FTAMeCases(const std::wstring& Info);
		virtual void ArrangeChildren() override;
		virtual std::wstring GetParseInfo() override;
		std::wstring EditInfo(const std::wstring& InInfo);

	public:
		void SetSymbolWidth(float InWidth);
		float GetSymbolWidth() const;
		wchar_t GetBracket() const;
		static std::wstring GetName();
	private:
		wchar_t Bracket = L'\0';
		float SymbolWidth = 0.0f;
	};
}
