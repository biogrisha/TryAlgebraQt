#pragma once
#include <Me/include/MeBase.h>

namespace TryAlgebraCore2
{
	class MeCharacter : public MeBase
	{
		TYPED_CLASS1(MeBase)
	public:
		MeCharacter(wchar_t ch);
	private:
		wchar_t m_ch = L'\0';
	};
}