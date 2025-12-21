// Fill out your copyright notice in the Description page of Project Settings.


#include "Modules/MathElementsV2/Me/MeIndex.h"

#include "FunctionLibraries/MeDefinition.h"


MathElementV2::FTAMeIndex::FTAMeIndex(const std::wstring& Info)
{
	if (Info == FTAMeDefinition::SubscriptInfo)
	{
		IndexType = EType::Subscript;
	}
	else if (Info == FTAMeDefinition::SuperscriptInfo)
	{
		IndexType = EType::Superscript;
	}
	else
	{
		IndexType = EType::Both;
	}
	SetDefaultSize({0.3f, IndexType != EType::Both ? 1.f/3.f : 1.f});
}

float MathElementV2::FTAMeIndex::GetScalingFactor(int ChildIndex)
{
	return GetReferenceScaler() / 1.5f;
}

void MathElementV2::FTAMeIndex::ArrangeChildren()
{
	if (IndexType == EType::Both)
	{
		if (bUseCustomParams)
		{
			Children[1]->SetLocalPosition({0, Children[0]->GetAbsoluteSize().y + CustomSubscriptOffset});
			return;
		}
		float ChildDist = DefaultSize.y - Children[0]->GetDefaultSize().y - Children[1]->GetDefaultSize().y;
		float VerticalOffset = Children[0]->GetAbsoluteSize().y + ChildDist;
		Children[1]->SetLocalPosition({0, VerticalOffset});
	}
}

std::wstring MathElementV2::FTAMeIndex::GetParseInfo()
{
	std::wstring Slash = L"\\";
	return Slash + GetName() + Slash + GetIndexTypeStr() + Slash;
}

float MathElementV2::FTAMeIndex::GetHorizontalAlignmentOffset() const
{
	if (bUseCustomParams)
	{
		return CustomHorizontalAlignment;
	}
	switch (IndexType)
	{
	case EType::Subscript:
		return -AbsoluteSize.y;
	case EType::Superscript:
		return AbsoluteSize.y;
	default: ;
	}
	return AbsoluteSize.y/2.f;
}

void MathElementV2::FTAMeIndex::SetCustomSubscriptOffset(float Height)
{
	if(IndexType == EType::Both)
	{
		CustomSubscriptOffset = (std::max)(Height, 0.f);
		Children[1]->SetLocalPosition({0,Children[0]->GetAbsoluteSize().y + CustomSubscriptOffset});
		CalculateCompSize();
	}
}

void MathElementV2::FTAMeIndex::UseCustomParams(bool bVal)
{
	bUseCustomParams = bVal;
	if (!bUseCustomParams)
	{
		ArrangeChildren();
		CalculateCompSize();
	}
}

std::wstring MathElementV2::FTAMeIndex::GetIndexTypeStr()
{
	switch (IndexType)
	{
	case EType::Subscript:
		return FTAMeDefinition::SubscriptInfo;
	case EType::Superscript:
		return FTAMeDefinition::SuperscriptInfo;
	case EType::Both:
		return FTAMeDefinition::DoublescriptInfo;
	default:
		return L"";
	}
}

MathElementV2::FTAMeIndex::EType MathElementV2::FTAMeIndex::GetIndexType() const
{
	return IndexType;
}

std::wstring MathElementV2::FTAMeIndex::GetName()
{
	return FTAMeDefinition::IndexName;
}
