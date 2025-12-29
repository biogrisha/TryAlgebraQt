#include "MathElementGenerator.h"
#include "Modules/MathElementsV2/Me/MeCharacter.h"
#include "Modules/Visual/VisualBase.h"
#include "Application.h"
#include "FreeTypeWrap.h"
#include "AppGlobal.h"
MathElementV2::FMathElementPtr MathElementGeneratorQt::CreateMathElement(const std::wstring& Name, const std::wstring& Info)
{
	return MathElementV2::FMathElementPtr();
}

MathElementV2::FMathElementPtr MathElementGeneratorQt::CreateCharacter(wchar_t Char)
{
	auto MeChar = MathElementV2::FTAMeCharacter::MakeTypedShared(Char);
	MeChar->SetVisual(std::make_shared<FTAVisual>());
	auto ft = AppGlobal::application->getFreeTypeWrap();
	auto size = ft->GetGlyphSize({ .Glyph = Char, .Height = 20 });
	MeChar->SetDefaultSize({ size.x,size.y });
	return MeChar;
}