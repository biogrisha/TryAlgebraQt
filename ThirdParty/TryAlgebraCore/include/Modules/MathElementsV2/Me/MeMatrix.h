// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MeComposite.h"

namespace MathElementV2
{
	class FTAMeMatrix : public FTAMeComposite
	{
		TYPED_CLASS_META(FTAMeMatrix,FTAMeComposite)
		protected:
		FTAMeMatrix(const std::wstring& Info);
	public:
		virtual std::wstring GetParseInfo() override;
		virtual void ArrangeChildren() override;
		virtual void AdjustChildrenSize() override;
		static std::wstring GetName();
		void GetSize(int& OutRows, int& OutColumns);
	protected:
		int Rows = 1;
		int Columns = 1;
		bool bCenterContent = true;
	};
}

