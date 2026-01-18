#include "MathElementGenerator.h"
#include <FreeTypeWrap.h>
#include <Modules/Visual/VisualBase.h>
#include <Modules/MathElementsV2/Me/MeCharacter.h>
#include <Modules/MathElementsV2/Me/MeNewLine.h>
#include <Modules/MathElementsV2/Me/MeContainer.h>
#include <Modules/MathElementsV2/Me/MeFromTo.h>

#include <Application.h>
#include <AppGlobal.h>
#include <MathEditor/Visual/CharVisual.h>
#include <MathEditor/Visual/FromToVisual.h>

MathElementV2::FMathElementPtr MathElementGeneratorQt::CreateMathElement(const std::wstring& Name, const std::wstring& Info)
{
	auto it = m_meGenerators.find(Name);
	if (it == m_meGenerators.end())
	{
		return nullptr;
	}
	return it->second(Info);
}

MathElementV2::FMathElementPtr MathElementGeneratorQt::CreateCharacter(wchar_t Char)
{
	auto ft = AppGlobal::application->getFreeTypeWrap();
	if (Char == L'\n' || Char == L'\r')
	{
		auto height = ft->GetHeightFromFontSize(15);
		auto newLine = MathElementV2::FTAMeNewLine::MakeTypedShared();
		newLine->SetDefaultSize({ 0, double(height) });
		return newLine;
	}
	if (Char == L' ')
	{
		auto height = ft->GetHeightFromFontSize(15);
		auto MeChar = MathElementV2::FTAMeCharacter::MakeTypedShared(Char);
		MeChar->SetVisual(std::make_shared<CharVisual>(MeChar.get(), m_meDocState));
		MeChar->SetDefaultSize({ 20,double(height) });
		return MeChar;
	}

	auto MeChar = MathElementV2::FTAMeCharacter::MakeTypedShared(Char);
	MeChar->SetVisual(std::make_shared<CharVisual>(MeChar.get(), m_meDocState));
	return MeChar;
}

void MathElementGeneratorQt::initMeGenerators()
{
	//Container
	{
		FuncCreateMe FuncCreateMe = [this](const std::wstring& Info)->MathElementV2::FMathElementPtr
			{
				auto Container = MathElementV2::FTAMeContainer::MakeTypedShared();
				auto MeVisual = std::make_shared<FTAVisual>();
				Container->SetVisual(MeVisual);
				return Container;
			};
		m_meGenerators.emplace(MathElementV2::FTAMeContainer::GetName(), FuncCreateMe);
	}
	//FromTo
	{
		FuncCreateMe FuncCreateMe = [this](const std::wstring& Info)->MathElementV2::FMathElementPtr
			{
				auto FromTo = MathElementV2::FTAMeFromTo::MakeTypedShared(Info);
				auto CharVisual = std::make_shared<FromToVisual>(FromTo.get(), m_meDocState);
				FromTo->SetVisual(CharVisual);
				return FromTo;
			};
		m_meGenerators.emplace(MathElementV2::FTAMeFromTo::GetName(), FuncCreateMe);
	}
}
