#pragma once
#include "MeComposite.h"


namespace MathElementV2
{
	class FTAMeParameter : public FTAMeComposite
	{
		TYPED_CLASS_META(FTAMeParameter,FTAMeComposite)
	public:
		enum class EType
		{
			Single,
			Multi,
			NonCommute
		};
	protected:
		FTAMeParameter(const std::wstring& InInfo);
		virtual void ArrangeChildren() override;
	public:
		virtual std::wstring GetParseInfo() override;
		static std::wstring GetName();
		EType GetParamType();
	private:
		std::wstring Info;
	};
}
