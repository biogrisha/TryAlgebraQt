// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "MeComposite.h"

namespace MathElementV2
{
	class FTAMeDocument;
	class FTAMeContainer : public FTAMeComposite
	{
		TYPED_CLASS_META(FTAMeContainer, FTAMeComposite)
	protected:
		FTAMeContainer();
		virtual void ArrangeChildren() override;
		virtual std::wstring GetParseInfo() override;
	public:
		void  SetOwnerDocument(const TTypedWeak<FTAMeDocument>& InDocument);
		static std::wstring GetName();
	protected:
		TTypedWeak<FTAMeDocument> OwnerDocument;
	};
}

