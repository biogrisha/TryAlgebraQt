#pragma once
#include "MathElementBaseV2.h"
#include "Modules/MathElementsV2/MathElementPath.h"

struct FTAMeHelpers;
namespace MathElementV2
{
	class FTAMeComposite : public FTAMathElementBase
	{
		TYPED_CLASS_META(FTAMeComposite, FTAMathElementBase)
		friend FTAMeHelpers; 

	protected:
		FTAMeComposite();
		//Adjust children size, s.t. in case of matrix, where you have to adjust containers size
		virtual void AdjustChildrenSize();
		virtual void ArrangeChildren();
		virtual void ScaleRecursive(float Height) override;
	public:
		virtual void ChildrenChanged(const FTAMePath& RequestPath, bool bSizeChanged);
		//What default size each child will have after being added
		virtual float GetScalingFactor(int ChildIndex) { return GetReferenceScaler(); };
	protected:
		virtual void Destroy() override;
	public:
		virtual std::wstring RemoveChildren(const FTAMePath& RequestPath, int Num);
		void AddChildren(const FTAMePath& RequestPath, const FMathElements& MathElements);
		void ScaleChildren();
		void CalculateSize();
		FMathElements& GetChildren() { return Children; }
		//Use this value in get scaling factor, e.g. GetReferenceScaler() * 3.f -> means that child will be 3 times smaller that origin
		float GetReferenceScaler() const;
	protected:
		void CalculateCompSize();
		void SetParentAndScale(const FTAMePath& RequestPath, const FMathElements& MathElements);
		TACommonTypes::FTAVector2d GetPadding() const;
		void SetAdjustChildrenSize(bool bVal);
		FMathElements Children;
		TACommonTypes::FTAVector2d Padding;
	private:
		bool bAdjustChildrenSize;

		//new logic
	public:
		virtual void CalculateSize(float AccumulatedScalingFactor) override;
		float GetAccumulatedScalingFactor();
	protected:
		//this.AccumulatedScalingFactor = parent.AccumulatedScalingFactor * this.AccumulatedScalingFactor
		float AccumulatedScalingFactor = 1;
	};
}
