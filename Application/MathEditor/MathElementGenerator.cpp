#include "MathElementGenerator.h"
#include "Modules/MathElementsV2/Me/MeCharacter.h"
#include "Modules/Visual/VisualBase.h"
MathElementV2::FMathElementPtr MathElementGeneratorQt::CreateMathElement(const std::wstring& Name, const std::wstring& Info)
{
	return MathElementV2::FMathElementPtr();
}

MathElementV2::FMathElementPtr MathElementGeneratorQt::CreateCharacter(wchar_t Char)
{
	auto MeChar = MathElementV2::FTAMeCharacter::MakeTypedShared(Char);
	MeChar->SetVisual(std::make_shared<FTAVisual>());
	MeChar->SetDefaultSize({ 1,1 });
	return MeChar;
}