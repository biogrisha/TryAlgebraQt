// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "MeComposite.h"

namespace MathElementV2
{
	class FTAMeFromTo : public FTAMeComposite
	{
		TYPED_CLASS_META(FTAMeFromTo, FTAMeComposite)
	protected:
		FTAMeFromTo(const std::wstring& InSymbol);
		virtual void ArrangeChildren() override;
		virtual void Scale(float ScalingFactor) override;
		virtual float GetScalingFactor(int ChildIndex) override;
		virtual std::wstring GetParseInfo() override;
		virtual float GetHorizontalAlignmentOffset() const override;
	public:
		const std::wstring& GetSymbol() const;
		const TACommonTypes::FTAVector2d& GetSymbolPosition() const;
		static std::wstring GetName();
		std::wstring Symbol;
		TACommonTypes::FTAVector2d SymbolPosition;
	};
}
