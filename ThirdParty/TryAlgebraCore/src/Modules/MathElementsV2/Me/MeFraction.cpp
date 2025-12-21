// Fill out your copyright notice in the Description page of Project Settings.


#include "Modules/MathElementsV2/Me/MeFraction.h"

#include "FunctionLibraries/MeDefinition.h"
#include "FunctionLibraries/MathElementsHelpers.h"
#include "Modules/MathElementsV2/Me/MeContainer.h"


MathElementV2::FTAMeFraction::FTAMeFraction()
{
	SetAdjustChildrenSize(true);
	Padding = {0.1f, 0.1f};
}

void MathElementV2::FTAMeFraction::ArrangeChildren()
{
	if (Children.size() < 2)
	{
		return;
	}
	//Align children horizontally and get their Center X
	FTAMeHelpers::AlignHorizontally(Children);
	auto Pos1 = Children[0]->GetLocalPosition();
	Pos1.y = 0;
	Children[0]->SetLocalPosition(Pos1);
	auto Pos2 = Children[1]->GetLocalPosition();
	Pos2.y = Children[0]->GetAbsoluteSize().y + GetSpaceBetweenContainers();
	Children[1]->SetLocalPosition(Pos2);
}

void MathElementV2::FTAMeFraction::AdjustChildrenSize()
{
	auto Size1 = Children[0]->GetAbsoluteSize();
	auto Size2 = Children[1]->GetAbsoluteSize();
	//Offset children to center them
	auto MinSizeChild = Size1.x < Size2.x ? Children[0] : Children[1];
	if (auto Cont = MinSizeChild->Cast<FTAMeContainer>())
	{
		FTAMeHelpers::OffsetMathElements(Cont->GetChildren(), {(std::abs)(Size1.x - Size2.x) / 2.f, 0});
	}

	float MaxX = (std::max)(Size1.x, Size2.x);
	Size1.x = MaxX;
	Size2.x = MaxX;
	Children[0]->SetAbsoluteSize(Size1);
	Children[1]->SetAbsoluteSize(Size2);
}

void MathElementV2::FTAMeFraction::ChildrenChanged(const FTAMePath& RequestPath, bool bSizeChanged)
{
	auto SizeTemp = AbsoluteSize;
	//Recalculate size, since containers adjusted
	CalculateSize();
	if (Parent.Get())
	{
		//If size changed -> signal to parent
		FTAMePath PathTemp = RequestPath;
		PathTemp.TreePath.pop_back();
		Parent->ChildrenChanged(PathTemp, SizeTemp != AbsoluteSize);
	}
}

float MathElementV2::FTAMeFraction::GetHorizontalAlignmentOffset() const
{
	return GetLineLocalPosition().y;
}

std::wstring MathElementV2::FTAMeFraction::GetParseInfo()
{
	std::wstring Slash = L"\\";
	return Slash + GetName() + Slash + Slash;
}

TACommonTypes::FTAVector2d MathElementV2::FTAMeFraction::GetLineSize() const
{
	return {GetAbsoluteSize().x - GetLinePadding() * 2,GetLineWidth()};
}

TACommonTypes::FTAVector2d MathElementV2::FTAMeFraction::GetLineLocalPosition() const
{
	float TopOffset = Children[0]->GetAbsoluteSize().y + Children[0]->GetLocalPosition().y;
	TopOffset += GetSpaceBetweenContainers() / 2;
	TopOffset -= GetLineWidth() / 2;
	return {GetLinePadding(), TopOffset};
}

float MathElementV2::FTAMeFraction::GetSpaceBetweenContainers() const
{
	return GetReferenceScaler() * 0.3f;
}

std::wstring MathElementV2::FTAMeFraction::GetName()
{
	return FTAMeDefinition::FractionName;
}

float MathElementV2::FTAMeFraction::GetLineWidth() const
{
	return GetReferenceScaler() * 0.07f;
}

float MathElementV2::FTAMeFraction::GetLinePadding() const
{
	return GetReferenceScaler() * 0.1f;
}
