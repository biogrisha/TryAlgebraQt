// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "MeCharacter.h"

namespace MathElementV2
{
	class FTAMeBracket : public FTAMeCharacter
	{
		TYPED_CLASS_META(FTAMeBracket,FTAMeCharacter)
	protected:
		FTAMeBracket(wchar_t Char);
	public:
		void SetCustomHorizontalAlignment(float Val);
		virtual float GetHorizontalAlignmentOffset() const override;
	protected:
		float CustomHorizontalAlignment = 0.f;
	};
}
