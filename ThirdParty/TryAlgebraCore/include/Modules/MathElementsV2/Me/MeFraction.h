// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "MeComposite.h"

namespace MathElementV2
{
	class FTAMeFraction : public FTAMeComposite
	{
		TYPED_CLASS_META(FTAMeFraction,FTAMeComposite)
	protected:
		FTAMeFraction();
		virtual void ArrangeChildren() override;
		virtual void AdjustChildrenSize() override;
		virtual void ChildrenChanged(const FTAMePath& RequestPath, bool bSizeChanged) override;
		virtual float GetHorizontalAlignmentOffset() const override;
	public:
		virtual std::wstring GetParseInfo() override;
		TACommonTypes::FTAVector2d GetLineSize() const;
		TACommonTypes::FTAVector2d GetLineLocalPosition() const;
		float GetSpaceBetweenContainers() const;
		static std::wstring GetName();
	private:
		float GetLineWidth() const;
		float GetLinePadding() const;
	};
}
