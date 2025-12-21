// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "MeComposite.h"


namespace MathElementV2
{
	class FTAMeCustomToken : public FTAMeComposite
	{
		TYPED_CLASS_META(FTAMeCustomToken, FTAMeComposite)
	protected:
		FTAMeCustomToken(const std::wstring& InTokenName);
		virtual void ArrangeChildren() override;
		virtual float GetHorizontalAlignmentOffset() const override;
	public:
		virtual std::wstring GetParseInfo() override;
		const std::wstring& GetTokenName() const;
		//Size assumed to have Y = 1
		void SetTokenNameSize(const TACommonTypes::FTAVector2d& InSize);
		TACommonTypes::FTAVector2d GetTokenSize() const;
		static std::wstring GetName();
	private:
		std::wstring TokenName;
		TACommonTypes::FTAVector2d TokenNameSize;
	};
}
