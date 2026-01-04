#pragma once
#include <functional>
#include <map>

#include "Modules/TypedObject.h"
#include "Modules/MathElementsV2/Me/MathElementBaseV2.h"
#include "Modules/MathElementsV2/MathElementPath.h"

namespace MathElementV2
{
	class FTAMeContainer;
	class FTAMeDocument;
	class FTAMeComposite;
	class FTAMeNewLine;
}

class FTransmitting_Me;
class FDocumentContainer_Me;
class FMathElementBase;
class FMathElementComposite;
struct FMathDebugNode;

struct FTAMeHelpers
{

	//-----------Me preparing-------------
	
	static void CalculateInitialScaling(const MathElementV2::FMathElements& MathElements);
	static void CalculateSize(const MathElementV2::FMathElements& MathElements);
	static void SetDepth(const MathElementV2::FMathElements& MathElements, int Depth);

	//------------Me arrangement------------
	
	static void AlignElementsVertically(const MathElementV2::FMathElements& MathElements, float& OutMinY, float& OutMaxVerticalOffset, bool bResetY = true);
	/**
	 * 
	 * @param MathElements
	 * @return y level at which elements are aligned
	 */
	static float AlignElementsVertically(const MathElementV2::FMathElements& MathElements, int From, int To, float MinY);
	static void ArrangeInLine(const MathElementV2::FMathElements& MathElements);
	static void ArrangeInLine(const MathElementV2::FMathElements& MathElements, int From, int To);
	//Aligns MEs  centers at the same x value. Returns this x value 
	static float AlignHorizontally(const MathElementV2::FMathElements& MathElements);
	static void ArrangeLines(MathElementV2::FTAMeDocument* InDocument, int From, int& OutTo);
	static void ArrangeLines(MathElementV2::FTAMeDocument* InDocument,float MinY, int From, int& OutTo);
	static void MoveMathElementsInY(const MathElementV2::FMathElements& MathElements, int From, int To, float Offset);
	static void MoveMathElementsInX(const MathElementV2::FMathElements& MathElements, float Offset);
	static void OffsetMathElements(const MathElementV2::FMathElements& MathElements, const TACommonTypes::FTAVector2d& Offset);
	static void ArrangeElementsInLines(MathElementV2::FTAMeDocument* InDocument, int& From, int& To);
	static void CalculateMeCountInLines(MathElementV2::FTAMeDocument* InDocument, int& From, int& To);

	//Assuming that containers are aligned vertically
	//This function will align their children elements
	static void AlignContentInContainersVertically(const MathElementV2::FMathElements& MathElements, int From, int To);
	//-------------Me finders----------
	
	static TTypedWeak<MathElementV2::FTAMeComposite> GetParentElement(MathElementV2::FTAMeComposite* InParent, const FTAMePath& Path, int MaxDepth = 100);
	static std::vector<TTypedWeak<MathElementV2::FTAMathElementBase>> GetMeRange(const MathElementV2::FMathElements& MathElements, int From, int To);
	static void GetIndexAtPosition(MathElementV2::FTAMeDocument* InDocument, const TACommonTypes::FTAVector2d& Position, FTAMePath& Path);
	static bool GetIndexAtPosition(const MathElementV2::FMathElements& MathElements, const TACommonTypes::FTAVector2d& Position, FTAMePath& Path);
	static int FindClosestIndex(const MathElementV2::FMathElements& MathElements, const TACommonTypes::FTAVector2d& Position);
	static int FindClosestIndex(const MathElementV2::FMathElements& MathElements, const TACommonTypes::FTAVector2d& Position, int From, int To);
	static int GetLastSameParentInd(const FTAMePath& InPath1, const FTAMePath& InPath2, FTAMePath& SmallerPath, FTAMePath& LargerPath);
	static int FindElementByX(const MathElementV2::FMathElements& MathElements, int From, float InX);

	static void FindMe(const std::vector<TTypedWeak<MathElementV2::FTAMathElementBase>>& Template, const MathElementV2::FMathElements& MathElements, int From, int To,std::vector<MathElementV2::FMathElements>& Result);
	static bool CompareMe(const std::vector<TTypedWeak<MathElementV2::FTAMathElementBase>>& Template, const MathElementV2::FMathElements& MathElements, int From);
	static bool CompareMe(const MathElementV2::FMathElements& Elements1, const MathElementV2::FMathElements& Elements2);
	static bool CompareMe(MathElementV2::FTAMathElementBase* Element1, MathElementV2::FTAMathElementBase* Element2);
	//-----------Me drawing------------
	
	static void DrawElementsDown(MathElementV2::FTAMeDocument* InDocument, int From, int To);
	static void SetShouldHide(const std::vector<TTypedWeak<MathElementV2::FTAMathElementBase>>& MathElements);
	static void HideIfShould(const std::vector<TTypedWeak<MathElementV2::FTAMathElementBase>>& MathElements);
	static void HideElements(const MathElementV2::FMathElements& MathElements);
	static void HideElements(const MathElementV2::FMathElements& MathElements, int From, int To);
	static void ShowElements(const MathElementV2::FMathElements& MathElements, int From, int To);
	static void ShowElements(const MathElementV2::FMathElements& MathElements);
	static void RedrawIfVisible(const std::vector<TTypedWeak<MathElementV2::FTAMathElementBase>>& MathElements);
	static void RedrawIfVisible(const MathElementV2::FMathElements& MathElements);
	static void HighlightContainers(const MathElementV2::FMathElements& MathElements);
	//-----------Parsing---------------
	
	static MathElementV2::FMathElements ParseFromString(const std::wstring& InString, class FTAMathElementGenerator* Generator);
	static std::wstring ParseToString(const std::vector<TTypedWeak<MathElementV2::FTAMathElementBase>>& MathElements);
	static std::wstring ParseToString(const MathElementV2::FMathElements& MathElements, int From, int To);
	static void ParseToString(const MathElementV2::FMathElements& MathElements, std::wstring& OutStr);
	static std::wstring GenContainers(int Count);
	//Key - Name, Val - MeData
	static const std::vector<std::pair<std::wstring, std::wstring>>& GetMathElementsList();
	
	//--------------Misc---------------------
	
	static bool IsWithinMe(const MathElementV2::FMathElementPtr& Me, const TACommonTypes::FTAVector2d& Position);
	static bool IsLeftHalf(const MathElementV2::FMathElementPtr& Me, const TACommonTypes::FTAVector2d& Position);
	static float ClosestDistSq(const MathElementV2::FMathElementPtr& Me, const TACommonTypes::FTAVector2d& Position);
	//--------------Lines---------------------
	
	static int GetLinesCount(const MathElementV2::FMathElements& AddedMe);
	static int GetLinesCount(const MathElementV2::FMathElements& MathElements, int From, int To);
	//Use this function to get line start, even if From == 0
	static MathElementV2::FTAMeNewLine* FindLineStart(MathElementV2::FTAMeDocument* InDocument, int From, int& OutLineStart);
	static MathElementV2::FTAMeNewLine* FindPrevLineStart(MathElementV2::FTAMeDocument* InDocument, int From, int& OutLineStart);
	static MathElementV2::FTAMeNewLine* FindNextLineStart(MathElementV2::FTAMeDocument* InDocument, int From, int& OutLineStart);
	//If found new line returns index after it(since you insert elements after new line)
	//otherwise returns 0
	static int FindPrevLineStartInd(MathElementV2::FTAMeDocument* InDocument, int From);
	//If found line, returns index after it, otherwise Children.size()
	static int FindNextLineStartInd(MathElementV2::FTAMeDocument* InDocument, int From);
	static void IterateOverLines(MathElementV2::FTAMeDocument* InDocument, int From, int To, const std::function<void(const MathElementV2::FMathElements&, int /*From*/,int /*To*/)>& Callable);
	//------------Selection----------------
	
	static std::vector<TTypedWeak<MathElementV2::FTAMathElementBase>> GetSelectedElements(MathElementV2::FTAMeDocument* InDocument,FTAMePath InPath1,FTAMePath InPath2, FTAMePath& OutCursorPos, bool& bPath1From);
	static void TrimPath(FTAMePath& OutPath, int NewSize);

	
	//----------Me navigation------------
	
	static int ScrollY(MathElementV2::FTAMeDocument* InDocument, int Count);
	static void MakeXStep(MathElementV2::FTAMeDocument* InDocument,FTAMePath& InPath, int Count);
	static void MakeYStep(MathElementV2::FTAMeDocument* InDocument,FTAMePath& InPath, int Count);
	static int DefaultStep(int Count, int Index, int ElementsCount);

private:
	static void StepOutsideContainer(MathElementV2::FTAMeContainer* InContainer, FTAMePath& InPath, int Count, bool bXDir);
private:
	static MathElementV2::FMathElements& ResultOrParentArray(MathElementV2::FMathElements& Result, MathElementV2::FTAMeComposite* Parent);
};

