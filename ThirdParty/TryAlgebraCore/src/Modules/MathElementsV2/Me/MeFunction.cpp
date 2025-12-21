// Fill out your copyright notice in the Description page of Project Settings.


#include "Modules/MathElementsV2/Me/MeFunction.h"

#include "FunctionLibraries/MathElementsHelpers.h"
#include "FunctionLibraries/MeDefinition.h"


MathElementV2::FTAMeFunction::FTAMeFunction()
{
	SetAdjustChildrenSize(true);
	Padding = {0.1f, 0.0f};
}

std::wstring MathElementV2::FTAMeFunction::GetParseInfo()
{
	std::wstring Slash = L"\\";
	return Slash + GetName() + Slash + Slash;
}

void MathElementV2::FTAMeFunction::ArrangeChildren()
{
	Children[0]->SetLocalPosition({0.f, 0.f});
	Children[1]->SetLocalPosition({Children[0]->GetAbsoluteSize().x, 0.f});
}

void MathElementV2::FTAMeFunction::AdjustChildrenSize()
{
	float MaxHeight = (std::max)(Children[0]->GetAbsoluteSize().y,Children[1]->GetAbsoluteSize().y);
	for (auto Child : Children)
	{
		float OldHeight = Child->GetAbsoluteSize().y;
		Child->SetAbsoluteSize({Child->GetAbsoluteSize().x, MaxHeight});
		if (auto Comp = Child->Cast<FTAMeComposite>())
		{
			FTAMeHelpers::MoveMathElementsInY(Comp->GetChildren(),0, Comp->GetChildren().size(), MaxHeight - OldHeight);
		}
	}
	FTAMeHelpers::AlignContentInContainersVertically(Children, 0, 2);
}

float MathElementV2::FTAMeFunction::GetHorizontalAlignmentOffset() const
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

std::wstring MathElementV2::FTAMeFunction::GetName()
{
	return FTAMeDefinition::FunctionName;
}
