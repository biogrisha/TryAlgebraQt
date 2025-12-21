// Fill out your copyright notice in the Description page of Project Settings.


#include "Modules/MathElementsV2/Me/MeHat.h"

#include "FunctionLibraries/MeDefinition.h"


MathElementV2::FTAMeHat::FTAMeHat(const std::wstring& InTextureName)
{
	TextureName = InTextureName;
	HatRelativeHeight = 0.4f;
	Padding = {0.1f,0.1f};
}

std::wstring MathElementV2::FTAMeHat::GetParseInfo()
{
	std::wstring Slash = L"\\";
	return Slash + GetName() + Slash + TextureName + Slash;
}

void MathElementV2::FTAMeHat::ArrangeChildren()
{
	Children[0]->SetLocalPosition({0,GetHatAbsoluteHeight()/4.f});
}

float MathElementV2::FTAMeHat::GetHorizontalAlignmentOffset() const
{
	if (auto Cont = Children[0]->Cast<FTAMeComposite>())
	{
		if (!Cont->GetChildren().empty())
		{
			auto& FirstContChild = Cont->GetChildren()[0];
			return  Cont->GetLocalPosition().y + FirstContChild->GetLocalPosition().y
			+ FirstContChild->GetHorizontalAlignmentOffset();
		}
	}
	return FTAMeComposite::GetHorizontalAlignmentOffset();
}

float MathElementV2::FTAMeHat::GetHatAbsoluteHeight() const
{
	return GetReferenceScaler() * HatRelativeHeight;
}

const std::wstring& MathElementV2::FTAMeHat::GetTextureName() const
{
	return TextureName;
}

std::wstring MathElementV2::FTAMeHat::GetName()
{
	return FTAMeDefinition::HatName;
}
