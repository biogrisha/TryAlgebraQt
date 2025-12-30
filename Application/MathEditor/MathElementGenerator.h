#pragma once
#include "MathDocumentRenderingStructs.h"
#include "Modules/MathElementsV2/CompatibilityData.h"
#include "Modules/MathElementsV2/Me/MathElementBaseV2.h"

class MathElementGeneratorQt : public FTAMathElementGenerator
{
	virtual MathElementV2::FMathElementPtr CreateMathElement(const std::wstring& Name, const std::wstring& Info) override;
	virtual MathElementV2::FMathElementPtr CreateCharacter(wchar_t Char) override;
public:
	std::vector<FGlyphData>* m_glyphsPtr = nullptr;
};

