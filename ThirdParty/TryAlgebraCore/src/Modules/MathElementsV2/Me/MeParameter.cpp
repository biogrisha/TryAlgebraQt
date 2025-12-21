// Fill out your copyright notice in the Description page of Project Settings.


#include "Modules/MathElementsV2/Me/MeParameter.h"

#include "FunctionLibraries/MeDefinition.h"


MathElementV2::FTAMeParameter::FTAMeParameter(const std::wstring& InInfo)
{
	Info = InInfo;
	Padding = {0.1f,0.0f};
}

void MathElementV2::FTAMeParameter::ArrangeChildren()
{
	Children[0]->SetLocalPosition({ 0,0 });
}

std::wstring MathElementV2::FTAMeParameter::GetParseInfo()
{
	std::wstring Slash = L"\\";
	return Slash + GetName() + Slash + Info + Slash;
}

std::wstring MathElementV2::FTAMeParameter::GetName()
{
	return FTAMeDefinition::ParamName;
}

MathElementV2::FTAMeParameter::EType MathElementV2::FTAMeParameter::GetParamType()
{
	if (Info == FTAMeDefinition::MultiParamInfo)
	{
		return EType::Multi;
	}
	return EType::Single;
}
