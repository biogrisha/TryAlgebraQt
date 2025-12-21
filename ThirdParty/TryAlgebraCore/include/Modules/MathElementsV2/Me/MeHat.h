// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "MeComposite.h"

namespace MathElementV2
{
	class FTAMeHat : public FTAMeComposite
	{
		TYPED_CLASS_META(FTAMeHat,FTAMeComposite)
	protected:
		FTAMeHat(const std::wstring& InTextureName);
	public:
		virtual std::wstring GetParseInfo() override;
		virtual void ArrangeChildren() override;
		virtual float GetHorizontalAlignmentOffset() const override;
		float GetHatAbsoluteHeight() const;
		const std::wstring& GetTextureName() const;
		static std::wstring GetName();
	private:
		std::wstring TextureName;
		float HatRelativeHeight;
	};
}
