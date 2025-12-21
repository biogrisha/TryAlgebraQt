// Fill out your copyright notice in the Description page of Project Settings.


#include "Modules/MathElementsV2/Me/MeCustomToken.h"

#include "FunctionLibraries/MeDefinition.h"


MathElementV2::FTAMeCustomToken::FTAMeCustomToken(const std::wstring& InTokenName)
{
	Padding = {0.1f,0.1f};
	TokenName = InTokenName;
}

void MathElementV2::FTAMeCustomToken::ArrangeChildren()
{
	//Make container offset Token name size, but slightly less
	float YOffset = GetTokenSize().y * 0.7f;
	Children[0]->SetLocalPosition({ 0,YOffset });
}

float MathElementV2::FTAMeCustomToken::GetHorizontalAlignmentOffset() const
{
	return FTAMeComposite::GetHorizontalAlignmentOffset();
}

std::wstring MathElementV2::FTAMeCustomToken::GetParseInfo()
{
	std::wstring Slash = L"\\";
	return Slash + GetName() + Slash + TokenName + Slash;
}

const std::wstring& MathElementV2::FTAMeCustomToken::GetTokenName() const
{
	return TokenName;
}

void MathElementV2::FTAMeCustomToken::SetTokenNameSize(const TACommonTypes::FTAVector2d& InSize)
{
	TokenNameSize = InSize / 2.f;
	auto DefSize = GetDefaultSize();
	DefSize.x = (std::max)(TokenNameSize.x, DefSize.x);
	SetDefaultSize(DefSize);
}

TACommonTypes::FTAVector2d MathElementV2::FTAMeCustomToken::GetTokenSize() const
{
	return TokenNameSize * GetReferenceScaler();
}

std::wstring MathElementV2::FTAMeCustomToken::GetName()
{
	return FTAMeDefinition::CustomTokenName;
}
