// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "MeComposite.h"

namespace MathElementV2
{
	class FTAMeTextBlock : public FTAMeComposite
	{
		TYPED_CLASS_META(FTAMeTextBlock, FTAMeComposite)
	protected:
		FTAMeTextBlock();

	public:
		virtual std::wstring GetParseInfo() override;
		virtual void ArrangeChildren() override;
		static std::wstring GetName();
	};
}
