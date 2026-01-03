// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include <string>
#include <vector>

#include "Modules/ComponentBase.h"
#include "Modules/CommonTypes/Vector2D.h"
class FTAVisual;
struct FTAMeHelpers;

namespace MathElementV2
{	
	class FTAMathElementBase : public FTAComponentsHost
	{
		TYPED_CLASS_META(FTAMathElementBase,FTAComponentsHost)
		friend class FTAMeComposite;
		friend FTAMeHelpers;
	public:
		virtual std::wstring GetParseInfo(){return {};}
		virtual void ScaleRecursive(float Height);
		virtual void Scale(float ScalingFactor);
		virtual float GetHorizontalAlignmentOffset() const;
		
		/**
		 * Set default size before Me added into the composite
		 * height of the character supposed to be 1
		 */
		TACommonTypes::FTAVector2d GetAbsolutePosition() const;
		TACommonTypes::FTAVector2d GetLocalPosition() const;
		const TACommonTypes::FTAVector2d& GetAbsoluteSize() const { return AbsoluteSize; }
		const TACommonTypes::FTAVector2d& GetDefaultSize() const { return DefaultSize; }
		const TTypedWeak<FTAMeComposite>& GetParent(){ return Parent; };
		int GetDepth() const {return Depth;}

		//Use this only for newly created elements to set initial sizes
		void SetDefaultSize(const TACommonTypes::FTAVector2d& InDefaultSize);
		//Use this function in AdjustChildren
		void SetAbsoluteSize(const TACommonTypes::FTAVector2d& InSize);
		void SetParent(const FTAMeComposite* InParent);
		void SetVisual(const std::shared_ptr<FTAVisual>& InVisual);
		void SetLocalPosition(const TACommonTypes::FTAVector2d& InLocalPosition);
		virtual void Destroy();
	protected:
		virtual void CalculateAbsolutePosition();
		TTypedWeak<FTAMeComposite> Parent;
		TACommonTypes::FTAVector2d LocalPosition;
		TACommonTypes::FTAVector2d DefaultSize;
		TACommonTypes::FTAVector2d AbsoluteSize;
		float InitialHeight = 1.f; 
		std::shared_ptr<FTAVisual> Visual;
		int Depth = 0;
		bool bShouldHide = true;
	public:
		bool bSelected = false;
		bool bHighlight = false;
		bool bSelectionMatchHighlight = false;

		//new logic
	public:
		virtual void CalculateSize(float AccumulatedScalingFactor);
	protected:
		bool bScaleFactorApplied = false;
	};

	using FMathElements = std::vector<std::shared_ptr<FTAMathElementBase>>;
	using FMathElementPtr = std::shared_ptr<FTAMathElementBase>;
}
