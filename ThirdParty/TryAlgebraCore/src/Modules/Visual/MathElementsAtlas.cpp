#include "Modules/Visual/MathElementsAtlas.h"
#include <string>
#include "FunctionLibraries/MathElementsHelpers.h"

void FTAMathElementsAtlas::Init(FTAMathElementGenerator* MeGenerator, int XExt, int& YExt)
{
	auto MeList = FTAMeHelpers::GetMathElementsList();
	MathElementV2::FMathElements MathElements(MeList.size());
	int Ind = 0;
	for (auto MeInfo : MeList)
	{
		MathElements[Ind] = FTAMeHelpers::ParseFromString(MeInfo.second, MeGenerator).back();
		Ind++;
	}
	FTAMeHelpers::CalculateSize(MathElements);

	int32_t YOffset = 0;
	int32_t XOffset = 0;
	int32_t MaxY = 0;
	for (auto& Me : MathElements)
	{
		if(XOffset)
		Me->SetLocalPosition({ double(XOffset), double(YOffset) });
		auto Size = Me->GetAbsoluteSize();
		XOffset += Size.x;
		MaxY = std::max<int>(Size.y, MaxY);

	}
}
