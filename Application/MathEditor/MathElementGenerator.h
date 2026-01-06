#pragma once
#include <functional>
#include <map>
#include "MathDocumentState.h"
#include "Modules/MathElementsV2/CompatibilityData.h"
#include "Modules/MathElementsV2/Me/MathElementBaseV2.h"

class MathElementGeneratorQt : public FTAMathElementGenerator
{
public:
	virtual MathElementV2::FMathElementPtr CreateMathElement(const std::wstring& Name, const std::wstring& Info) override;
	virtual MathElementV2::FMathElementPtr CreateCharacter(wchar_t Char) override;
	void initMeGenerators();

	FMathDocumentState* m_meDocState = nullptr;
	using FuncCreateMe = std::function<MathElementV2::FMathElementPtr(const std::wstring& Info)>;
	std::map<std::wstring, FuncCreateMe> m_meGenerators;
};

