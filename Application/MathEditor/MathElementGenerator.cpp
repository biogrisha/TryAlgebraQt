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
		MeChar->SetVisual(m_chVisual.get());
		MeChar->SetDefaultSize({ 20,double(height) });
		return MeChar;
	}

	auto MeChar = MathElementV2::FTAMeCharacter::MakeTypedShared(Char);
	MeChar->SetVisual(m_chVisual.get());
	return MeChar;
}

void MathElementGeneratorQt::initMeGenerators()
{
	m_ftVisual = std::make_unique<FromToVisual>(m_meDocState);
	m_chVisual = std::make_unique<CharVisual>(m_meDocState);
	m_visualBase = std::make_unique<MeVisualBase>(m_meDocState);
	//Container
	{
		FuncCreateMe FuncCreateMe = [this](const std::wstring& Info)->MathElementV2::FMathElementPtr
			{
				auto Container = MathElementV2::FTAMeContainer::MakeTypedShared();
				Container->SetVisual(m_visualBase.get());
				return Container;
			};
		m_meGenerators.emplace(MathElementV2::FTAMeContainer::GetName(), FuncCreateMe);
	}
	//FromTo
	{
		FuncCreateMe FuncCreateMe = [this](const std::wstring& Info)->MathElementV2::FMathElementPtr
			{
				auto FromTo = MathElementV2::FTAMeFromTo::MakeTypedShared(Info);
				FromTo->SetVisual(m_ftVisual.get());
				return FromTo;
			};
		m_meGenerators.emplace(MathElementV2::FTAMeFromTo::GetName(), FuncCreateMe);
	}
}
