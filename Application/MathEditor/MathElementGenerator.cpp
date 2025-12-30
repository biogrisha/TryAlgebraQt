#include "MathElementGenerator.h"
#include "Modules/MathElementsV2/Me/MeCharacter.h"
#include "Modules/MathElementsV2/Me/MeNewLine.h"
#include "Modules/Visual/VisualBase.h"
#include "Application.h"
#include "FreeTypeWrap.h"
#include "AppGlobal.h"
#include "Visual/CharVisual.h"

MathElementV2::FMathElementPtr MathElementGeneratorQt::CreateMathElement(const std::wstring& Name, const std::wstring& Info)
{
	return MathElementV2::FMathElementPtr();
}

MathElementV2::FMathElementPtr MathElementGeneratorQt::CreateCharacter(wchar_t Char)
{
	if (Char == L'\n' || Char == L'\r')
	{
		return MathElementV2::FTAMeNewLine::MakeTypedShared();
	}
	if (Char == L' ')
	{
		auto MeChar = MathElementV2::FTAMeCharacter::MakeTypedShared(Char);
		MeChar->SetVisual(std::make_shared<CharVisual>(MeChar.get(), nullptr));
		MeChar->SetDefaultSize({ 20,0 });
		return MeChar;
	}

	auto MeChar = MathElementV2::FTAMeCharacter::MakeTypedShared(Char);
	MeChar->SetVisual(std::make_shared<CharVisual>(MeChar.get(), m_glyphsPtr));
	auto ft = AppGlobal::application->getFreeTypeWrap();
	auto size = ft->GetGlyphSize({ .Glyph = Char, .Height = 20 });
	MeChar->SetDefaultSize({ size.x,size.y });
	return MeChar;
}