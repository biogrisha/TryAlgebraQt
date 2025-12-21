// Fill out your copyright notice in the Description page of Project Settings.


#include "Modules/MathElementsV2/Me/MeComposite.h"
#include "FunctionLibraries/MathElementsHelpers.h"


MathElementV2::FTAMeComposite::FTAMeComposite()
{
	SetDefaultSize({ 0.5f, 1.f });
	bAdjustChildrenSize = false;
}

void MathElementV2::FTAMeComposite::AdjustChildrenSize()
{
}

void MathElementV2::FTAMeComposite::ArrangeChildren()
{
}

void MathElementV2::FTAMeComposite::ScaleRecursive(float Height)
{
	Scale(Height);
	for (auto& Child : Children)
	{
		Child->ScaleRecursive(Height);
	}
}

void MathElementV2::FTAMeComposite::ChildrenChanged(const FTAMePath& RequestPath,bool bSizeChanged)
{
	if (bAdjustChildrenSize)
	{
		auto SizeTemp = AbsoluteSize;
		//Recalculate size, since all containers were adjusted
		CalculateSize();
		if (Parent.Get())
		{
			//If size changed -> signal to parent
			FTAMePath PathTemp = RequestPath;
			PathTemp.TreePath.pop_back();
			Parent->ChildrenChanged(PathTemp,SizeTemp != AbsoluteSize);
		}
	}
	else
	{
		//Arrange children
		ArrangeChildren();
		//Offset children with respect to padding
		FTAMeHelpers::OffsetMathElements(Children, GetPadding());
		//Cache size
		auto SizeTemp = AbsoluteSize;
		//Calculate the size so that the element covers all its child elements
		CalculateCompSize();
		if (Parent.Get())
		{
			//If size changed -> signal to parent
			FTAMePath PathTemp = RequestPath;
			PathTemp.TreePath.pop_back();
			Parent->ChildrenChanged(PathTemp,SizeTemp != AbsoluteSize);
		}
	}
}

void MathElementV2::FTAMeComposite::AddChildren(const FTAMePath& RequestPath, const FMathElements& MathElements)
{
	//Scale children
	//We assume that children were just created using parser, and they have default sizes
	SetParentAndScale(RequestPath,MathElements);
	//insert children in last index in path
	Children.insert(Children.begin() + RequestPath.TreePath.back(), MathElements.begin(), MathElements.end());
	ChildrenChanged(RequestPath, true);
}

void MathElementV2::FTAMeComposite::Destroy()
{
	FTAMathElementBase::Destroy();
	for (auto& Child : Children)
	{
		Child->Destroy();
	}
}

std::wstring MathElementV2::FTAMeComposite::RemoveChildren(const FTAMePath& RequestPath, int Num)
{
	int From = RequestPath.TreePath.back();
	int To = From + Num;
	std::wstring CachedMe = FTAMeHelpers::ParseToString(Children, From, To);
	for (int i = From; i < To; i++)
	{
		Children[i]->Destroy();
	}
	Children.erase(Children.begin() + From, Children.begin() + To);
	ChildrenChanged(RequestPath, true);
	return CachedMe;
}

void MathElementV2::FTAMeComposite::ScaleChildren()
{
	int ChildIndex = 0;
	for (auto& Child : Children)
	{
		//Scale children with scaling factor
		Child->Scale(GetScalingFactor(ChildIndex));
		if (auto Comp = Child->Cast<FTAMeComposite>())
		{
			//if child also composite-> apply recursion
			Comp->ScaleChildren();
		}
		++ChildIndex;
	}
}

void MathElementV2::FTAMeComposite::CalculateSize()
{
	//Calculate children size recursively
	for (auto& Child : Children)
	{
		if (auto Comp = Child->Cast<FTAMeComposite>())
		{
			Comp->CalculateSize();
		}
	}
	if (bAdjustChildrenSize)
	{
		//Adjust children size
		AdjustChildrenSize();
	}
	//Arrange children
	ArrangeChildren();
	//Offset children with respect to padding
	FTAMeHelpers::OffsetMathElements(Children, GetPadding());
	//Calculate size such that element would embrace all children
	CalculateCompSize();
}

float MathElementV2::FTAMeComposite::GetReferenceScaler() const
{
	return DefaultSize.y/InitialHeight;
}

void MathElementV2::FTAMeComposite::CalculateCompSize()
{
	AbsoluteSize = DefaultSize;
	for (auto& Child : Children)
	{
		auto ExtremePoint = Child->LocalPosition + Child->AbsoluteSize;
		AbsoluteSize = TACommonTypes::Max(AbsoluteSize, ExtremePoint);
	}
	AbsoluteSize += GetPadding();
}

void MathElementV2::FTAMeComposite::SetParentAndScale(const FTAMePath& RequestPath, const FMathElements& MathElements)
{
	int MeIndex = RequestPath.TreePath.back();
	for (auto& Me : MathElements)
	{
		//Set parent
		Me->SetParent(this);
		//Scale children with scaling factor
		Me->ScaleRecursive(GetScalingFactor(MeIndex));
		++MeIndex;
	}
}

TACommonTypes::FTAVector2d MathElementV2::FTAMeComposite::GetPadding() const
{
	return Padding * GetReferenceScaler();
}

void MathElementV2::FTAMeComposite::SetAdjustChildrenSize(bool bVal)
{
	bAdjustChildrenSize = bVal;
}


