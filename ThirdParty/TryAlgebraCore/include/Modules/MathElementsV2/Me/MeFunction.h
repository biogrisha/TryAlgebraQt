// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MeComposite.h"

namespace MathElementV2
{
	class FTAMeFunction : public FTAMeComposite
	{
		TYPED_CLASS_META(FTAMeFunction,FTAMeComposite)
		protected:
		FTAMeFunction();
	public:
		virtual std::wstring GetParseInfo() override;
		virtual void ArrangeChildren() override;
		virtual void AdjustChildrenSize() override;
		virtual float GetHorizontalAlignmentOffset() const override;
		static std::wstring GetName();
	};
}
