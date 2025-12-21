// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "MeComposite.h"

namespace MathElementV2
{
	class FTAMeIndex : public FTAMeComposite
	{
		TYPED_CLASS_META(FTAMeIndex,FTAMeComposite)
	public:
		enum class EType
		{
			Subscript,
			Superscript,
			Both
		};
	protected:
		FTAMeIndex(const std::wstring& Info);
	public:
		virtual float GetScalingFactor(int ChildIndex) override;
		virtual void ArrangeChildren() override;
		virtual std::wstring GetParseInfo() override;
		virtual float GetHorizontalAlignmentOffset() const override;
		//This function used to adjust index position depending on the prev Me
		//The distance between the bottom of the superscript and the top of the subscript
		void SetCustomSubscriptOffset(float Height);
		void SetCustomAlign(float Val) { CustomHorizontalAlignment = Val; }
		void UseCustomParams(bool bVal);
		std::wstring GetIndexTypeStr();
		EType GetIndexType() const;
		static std::wstring GetName();
	private:
		bool bUseCustomParams = false;
		float CustomHorizontalAlignment = 0;
		float CustomSubscriptOffset = 0;
		EType IndexType = EType::Subscript;
	};
}
