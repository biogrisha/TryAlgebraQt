#include "FunctionLibraries/MathElementsHelpers.h"

#include <cassert>
#include <algorithm>
#include "FunctionLibraries/CommonHelpers.h"
#include "FunctionLibraries/MeDefinition.h"
#include "Modules/MathElementsV2/MeStepComponent.h"
#include "Modules/MathElementsV2/Me/MeCases.h"
#include "Modules/MathElementsV2/Me/MeComposite.h"
#include "Modules/MathElementsV2/Me/MeContainer.h"
#include "Modules/MathElementsV2/Me/MeDocument.h"
#include "Modules/MathElementsV2/Me/MeFraction.h"
#include "Modules/MathElementsV2/Me/MeFromTo.h"
#include "Modules/MathElementsV2/Me/MeFunction.h"
#include "Modules/MathElementsV2/Me/MeHat.h"
#include "Modules/MathElementsV2/Me/MeIndex.h"
#include "Modules/MathElementsV2/Me/MeMatrix.h"
#include "Modules/MathElementsV2/Me/MeNewLine.h"
#include "Modules/MathElementsV2/Me/MeCustomToken.h"
#include "Modules/MathElementsV2/Me/MeParameter.h"
#include "Modules/MathElementsV2/Me/MeSpecialSymbol.h"
#include "Modules/MathElementsV2/Me/MeTextBlock.h"
#include "Modules/Visual/VisualBase.h"


TTypedWeak<MathElementV2::FTAMeComposite> FTAMeHelpers::GetParentElement(MathElementV2::FTAMeComposite* InParent, const FTAMePath& Path, int MaxDepth)
{
	TTypedWeak<MathElementV2::FTAMeComposite> TempParent = InParent->GetSelfWeak();
	int UpToInd = (std::min)(static_cast<int>(Path.TreePath.size() - 1), MaxDepth);
	for (int i = 0; i < UpToInd; i++)
	{
		int Index = Path.TreePath[i];
		if (!CommonHelpers::IsValidId(TempParent->Children, Index))
		{
			return TTypedWeak<MathElementV2::FTAMeComposite>();
		}
		auto Child = TempParent->Children[Index];
		MathElementV2::FTAMeComposite* Comp = Child->Cast<MathElementV2::FTAMeComposite>();
		if (!Comp)
		{
			return TTypedWeak<MathElementV2::FTAMeComposite>();
		}
		TempParent = Comp->GetSelfWeak();
	}
	return TempParent;
}

void FTAMeHelpers::CalculateInitialScaling(const MathElementV2::FMathElements& MathElements)
{
	for (auto& Element : MathElements)
	{
		if (auto Comp = Element->Cast<MathElementV2::FTAMeComposite>())
		{
			Comp->ScaleChildren();
		}
	}
}

void FTAMeHelpers::CalculateSize(const MathElementV2::FMathElements& MathElements)
{
	for (auto& Element : MathElements)
	{
		Element->CalculateSize(1);
	}
}

void FTAMeHelpers::OffsetMathElements(const MathElementV2::FMathElements& MathElements, const TACommonTypes::FTAVector2d& Offset)
{
	for (auto& Element : MathElements)
	{
		Element->LocalPosition += Offset;
	}
}

void FTAMeHelpers::AlignElementsVertically(const MathElementV2::FMathElements& MathElements, float& OutMinY, float& OutMaxVerticalOffset, bool bResetY)
{
	//Find upper bound
	float MinY = bResetY ? 0.f : 99999999.f;
	if (!bResetY)
	{
		for (auto& Element : MathElements)
		{
			MinY = (std::min)(Element->LocalPosition.y, MinY);
		}
	}

	//Align them by upper edge
	//And find max offset
	float MaxOffset = 0;
	for (auto& Element : MathElements)
	{
		float Offset = Element->GetHorizontalAlignmentOffset();
		Element->LocalPosition.y = MinY - Offset;
		MaxOffset = (std::max)(Offset, MaxOffset);
	}
	//add max offset
	for (auto& Element : MathElements)
	{
		Element->LocalPosition.y += MaxOffset;
	}
	OutMinY = MinY;
	OutMaxVerticalOffset = MaxOffset;
}

float FTAMeHelpers::AlignElementsVertically(const MathElementV2::FMathElements& MathElements, int From, int To, float MinY)
{
	//Align them by upper edge
	//And find max offset
	float MaxOffset = 0;
	for (int i = From; i < To; i++)
	{
		float Offset = MathElements[i]->GetHorizontalAlignmentOffset();
		MathElements[i]->LocalPosition.y = MinY - Offset;
		MaxOffset = (std::max)(Offset, MaxOffset);
	}
	//add max offset
	float MaxY = 0;
	for (int i = From; i < To; i++)
	{
		MathElements[i]->LocalPosition.y += MaxOffset;
		MaxY = (std::max)(MathElements[i]->LocalPosition.y + MathElements[i]->AbsoluteSize.y, MaxY);
	}
	return MaxY - MinY;
}


void FTAMeHelpers::ArrangeInLine(const MathElementV2::FMathElements& MathElements)
{
	float NextXPos = 0.f;
	for (auto& Element : MathElements)
	{
		Element->LocalPosition.x = NextXPos;
		NextXPos = Element->LocalPosition.x + Element->AbsoluteSize.x;
	}
}

void FTAMeHelpers::ArrangeInLine(const MathElementV2::FMathElements& MathElements, int From, int To)
{
	float NextXPos = 0.f;
	for (int i = From; i < To; i++)
	{
		MathElements[i]->LocalPosition.x = NextXPos;
		NextXPos = MathElements[i]->LocalPosition.x + MathElements[i]->AbsoluteSize.x;
	}
}

float FTAMeHelpers::AlignHorizontally(const MathElementV2::FMathElements& MathElements)
{
	float MaxHalfSize = 0.f;
	for (auto& Me : MathElements)
	{
		float HalfSize = Me->AbsoluteSize.x / 2.f;
		MaxHalfSize = (std::max)(MaxHalfSize, HalfSize);
		Me->LocalPosition.x = -HalfSize;
	}
	for (auto& Me : MathElements)
	{
		Me->LocalPosition.x += MaxHalfSize;
	}
	return MaxHalfSize;
}

void FTAMeHelpers::SetDepth(const MathElementV2::FMathElements& MathElements, int Depth)
{
	for (auto& Element : MathElements)
	{
		if (auto Comp = Element->Cast<MathElementV2::FTAMeComposite>())
		{
			SetDepth(Comp->Children, Depth + 1);
		}
		Element->Depth = Depth;
	}
}

void FTAMeHelpers::ArrangeLines(MathElementV2::FTAMeDocument* InDocument, int From, int& OutTo)
{
	float MinY = 0;
	int PrevLineStart = 0;
	if (MathElementV2::FTAMeNewLine* PrevLine = FindPrevLineStart(InDocument, From, PrevLineStart))
	{
		//If there is line above
		//MinY is its bottom
		MinY = PrevLine->MinY + PrevLine->Height;
	}
	ArrangeLines(InDocument, MinY, From, OutTo);
}

void FTAMeHelpers::ArrangeLines(MathElementV2::FTAMeDocument* InDocument, float MinY, int From, int& OutTo)
{
	for (int i = From; i < InDocument->Children.size();)
	{
		if (MinY >= InDocument->RelativeHeight)
		{
			//If Line height is below screen
			OutTo = i;
			return;
		}
		//Get line
		MathElementV2::FTAMeNewLine* Line = FindLineStart(InDocument, i, i);
		//How much we need to lift it or lower
		float HeightDiff = MinY - Line->MinY;
		//Move line correspondingly
		MoveMathElementsInY(InDocument->Children, i, i + Line->ElementsCount, HeightDiff);
		Line->MinY = MinY;
		//Set next line MinY
		MinY = Line->MinY + Line->Height;
		i = i + Line->ElementsCount;
	}
	OutTo = InDocument->Children.size();
}

void FTAMeHelpers::MoveMathElementsInY(const MathElementV2::FMathElements& MathElements, int From, int To, float Offset)
{
	for (int i = From; i < To; i++)
	{
		MathElements[i]->LocalPosition.y += Offset;
	}
}

void FTAMeHelpers::MoveMathElementsInX(const MathElementV2::FMathElements& MathElements, float Offset)
{
	for (auto& Me : MathElements)
	{
		Me->LocalPosition.x += Offset;
	}
}

void FTAMeHelpers::ArrangeElementsInLines(MathElementV2::FTAMeDocument* InDocument, int& From, int& To)
{
	for (int i = From; i < To;)
	{
		MathElementV2::FTAMeNewLine* CurrentLine = FindLineStart(InDocument, i, i);

		CurrentLine->Height = AlignElementsVertically(InDocument->Children, i, i + CurrentLine->ElementsCount, CurrentLine->MinY);
		ArrangeInLine(InDocument->Children, i, i + CurrentLine->ElementsCount);
		i += CurrentLine->ElementsCount;
	}
}

MathElementV2::FTAMeNewLine* FTAMeHelpers::FindLineStart(MathElementV2::FTAMeDocument* InDocument, int From, int& OutLineStart)
{
	if (From < 0)
	{
		OutLineStart = -1;
		return nullptr;
	}
	From = (std::min)(From, static_cast<int>(InDocument->Children.size() - 1));
	for (int i = From; i >= 0; i--)
	{
		if (auto NewLine = InDocument->Children[i]->Cast<MathElementV2::FTAMeNewLine>())
		{
			OutLineStart = i;
			return NewLine;
		}
	}
	//We assume that 0-th element is always line start
	return nullptr;
}

MathElementV2::FTAMeNewLine* FTAMeHelpers::FindPrevLineStart(MathElementV2::FTAMeDocument* InDocument, int From, int& OutLineStart)
{
	if (InDocument->Children.empty())
	{
		OutLineStart = -1;
		return nullptr;
	}
	//Find current line start
	int OutLineStartTemp = 0;
	MathElementV2::FTAMeNewLine* LineStart = FindLineStart(InDocument, From, OutLineStartTemp);
	if (OutLineStartTemp == 0)
	{
		return nullptr;
	}
	//If non zero index return previous line
	return FindLineStart(InDocument, OutLineStartTemp - 1, OutLineStart);
}

int FTAMeHelpers::FindPrevLineStartInd(MathElementV2::FTAMeDocument* InDocument, int From)
{
	if (InDocument->Children.empty())
	{
		return 0;
	}

	From = std::min(int(InDocument->Children.size() - 1), From);
	bool bSkipFirstLine = false;
	for (int i = From; i >= 0; i--)
	{
		if (InDocument->Children[i]->IsOfType(MathElementV2::FTAMeNewLine::StaticType()))
		{
			if (bSkipFirstLine)
			{
				return i + 1;
			}
			else
			{
				bSkipFirstLine = true;
			}
		}
	}
	return 0;
}

int FTAMeHelpers::FindNextLineStartInd(MathElementV2::FTAMeDocument* InDocument, int From)
{
	if (InDocument->Children.empty())
	{
		return 0;
	}
	for (int i = From; i < InDocument->Children.size(); i++)
	{
		if (InDocument->Children[i]->IsOfType(MathElementV2::FTAMeNewLine::StaticType()))
		{
			return i + 1;
		}
	}
	return InDocument->Children.size();
}

MathElementV2::FTAMeNewLine* FTAMeHelpers::FindNextLineStart(MathElementV2::FTAMeDocument* InDocument, int From, int& OutLineStart)
{
	if (!CommonHelpers::IsValidId(InDocument->Children, From))
	{
		return nullptr;
	}
	if (auto NewLine = InDocument->Children[From]->Cast<MathElementV2::FTAMeNewLine>())
	{
		//If From points at new line
		OutLineStart = From + NewLine->ElementsCount;
		if (!CommonHelpers::IsValidId(InDocument->Children, OutLineStart))
		{
			return nullptr;
		}
		return InDocument->Children[OutLineStart]->Cast<MathElementV2::FTAMeNewLine>();
	}
	for (int i = From; i < InDocument->Children.size(); i++)
	{
		if (auto NewLine = InDocument->Children[i]->Cast<MathElementV2::FTAMeNewLine>())
		{
			OutLineStart = i;
			return NewLine;
		}
	}
	OutLineStart = InDocument->Children.size();
	return nullptr;
}

void FTAMeHelpers::IterateOverLines(MathElementV2::FTAMeDocument* InDocument, int From, int To, const std::function<void(const MathElementV2::FMathElements&, int, int)>& Callable)
{
	for (int i = From; i < To;)
	{
		auto NewLine = FindLineStart(InDocument, i, i);
		Callable(InDocument->Children, i, i + NewLine->ElementsCount);
		i += NewLine->ElementsCount;
	}
}

void FTAMeHelpers::CalculateMeCountInLines(MathElementV2::FTAMeDocument* InDocument, int& From, int& To)
{
	//Cache first line
	MathElementV2::FTAMeNewLine* CurrentLine = FindLineStart(InDocument, From, From);
	int CurrentLineCount = 0;
	//Iterate from 
	for (int i = From; i < InDocument->Children.size(); i++)
	{
		if (auto NewLine = InDocument->Children[i]->Cast<MathElementV2::FTAMeNewLine>(); NewLine && NewLine != CurrentLine)
		{
			//If new line
			//Set count and cache new line
			CurrentLine->ElementsCount = CurrentLineCount;
			CurrentLineCount = 0;
			CurrentLine = NewLine;
			if (i >= To)
			{
				//If the new line comes after the last element
				To = i;
				return;
			}
		}
		CurrentLineCount++;
	}
	CurrentLine->ElementsCount = CurrentLineCount;
	To = InDocument->Children.size();
}

void FTAMeHelpers::AlignContentInContainersVertically(const MathElementV2::FMathElements& MathElements, int From, int To)
{
	//Find the highest align point
	float HighestAlignPoint = FLT_MAX;
	for (int i = From; i < To; i++)
	{
		if (auto Cont = MathElements[i]->Cast<MathElementV2::FTAMeContainer>())
		{
			if (Cont->Children.empty())
			{
				continue;
			}
			float AlignPoint = Cont->Children[0]->GetHorizontalAlignmentOffset()
				+ Cont->Children[0]->GetLocalPosition().y;
			HighestAlignPoint = (std::min)(AlignPoint, HighestAlignPoint);
		}
	}
	//Raise the math symbols by the height difference between the points
	for (int i = From; i < To; i++)
	{
		if (auto Cont = MathElements[i]->Cast<MathElementV2::FTAMeContainer>())
		{
			if (Cont->Children.empty())
			{
				continue;
			}
			float AlignPoint = Cont->Children[0]->GetHorizontalAlignmentOffset()
				+ Cont->Children[0]->GetLocalPosition().y;
			MoveMathElementsInY(Cont->Children, 0, Cont->Children.size(), HighestAlignPoint - AlignPoint);
		}
	}
}

bool FTAMeHelpers::CompareMe(const std::vector<TTypedWeak<MathElementV2::FTAMathElementBase>>& Template, const MathElementV2::FMathElements& MathElements, int From)
{
	if (Template.size() > MathElements.size() - From)
	{
		//Compare sizes
		return false;
	}
	for (int i = 0; i < Template.size(); i++)
	{
		//for each template symbol
		//Compare symbols
		if (!CompareMe(Template[i].Get(), MathElements[i + From].get()))
		{
			return false;
		}
	}
	//Compare children
	for (int i = 0; i < Template.size(); i++)
	{
		if (auto AsComposite1 = Template[i]->Cast<MathElementV2::FTAMeComposite>())
		{
			if (auto AsComposite2 = MathElements[i + From]->Cast<MathElementV2::FTAMeComposite>())
			{
				if (!CompareMe(AsComposite1->GetChildren(), AsComposite2->GetChildren()))
				{
					return false;
				}
			}
		}
	}
	return true;
}

bool FTAMeHelpers::CompareMe(const MathElementV2::FMathElements& Elements1, const MathElementV2::FMathElements& Elements2)
{
	if (Elements1.size() != Elements2.size())
	{
		return false;
	}
	for (int i = 0; i < Elements1.size(); i++)
	{
		if (!CompareMe(Elements1[i].get(), Elements2[i].get()))
		{
			return false;
		}
	}
	for (int i = 0; i < Elements1.size(); i++)
	{
		if (auto AsComposite1 = Elements1[i]->Cast<MathElementV2::FTAMeComposite>())
		{
			if (auto AsComposite2 = Elements2[i]->Cast<MathElementV2::FTAMeComposite>())
			{
				if (!CompareMe(AsComposite1->GetChildren(), AsComposite2->GetChildren()))
				{
					return false;
				}
			}
		}
	}
	return true;
}

bool FTAMeHelpers::CompareMe(MathElementV2::FTAMathElementBase* Element1, MathElementV2::FTAMathElementBase* Element2)
{
	return Element1->GetParseInfo() == Element2->GetParseInfo();
}

void FTAMeHelpers::DrawElementsDown(MathElementV2::FTAMeDocument* InDocument, int From, int To)
{
	for (int i = From; i < To; i++)
	{
		InDocument->Children[i]->Visual->Show();
	}
}

std::vector<TTypedWeak<MathElementV2::FTAMathElementBase>> FTAMeHelpers::GetMeRange(const MathElementV2::FMathElements& MathElements, int From, int To)
{
	std::vector<TTypedWeak<MathElementV2::FTAMathElementBase>> MeRange;
	for (int i = From; i < To; i++)
	{
		MeRange.push_back(MathElements[i]->GetSelfWeak());
	}
	return MeRange;
}

void FTAMeHelpers::SetShouldHide(const std::vector<TTypedWeak<MathElementV2::FTAMathElementBase>>& MathElements)
{
	for (auto Me : MathElements)
	{
		Me->bShouldHide = true;
	}
}

void FTAMeHelpers::HideIfShould(const std::vector<TTypedWeak<MathElementV2::FTAMathElementBase>>& MathElements)
{
	for (auto Me : MathElements)
	{
		if (Me->bShouldHide)
		{
			if (Me->Visual)
			{
				Me->Visual->Hide();
			}
			if (auto Comp = Me->Cast<MathElementV2::FTAMeComposite>())
			{
				HideElements(Comp->Children);
			}
		}
	}
}

void FTAMeHelpers::HideElements(const MathElementV2::FMathElements& MathElements)
{
	HideElements(MathElements, 0, MathElements.size());
}

void FTAMeHelpers::HideElements(const MathElementV2::FMathElements& MathElements, int From, int To)
{
	for (int i = From; i < To; i++)
	{
		if (MathElements[i]->Visual)
		{
			MathElements[i]->Visual->Hide();
		}
		if (auto Comp = MathElements[i]->Cast<MathElementV2::FTAMeComposite>())
		{
			HideElements(Comp->Children);
		}
	}
}

void FTAMeHelpers::ShowElements(const MathElementV2::FMathElements& MathElements, int From, int To)
{
	for (int i = From; i < To; i++)
	{
		if (MathElements[i]->Visual)
		{
			MathElements[i]->Visual->Show();
		}
		MathElements[i]->bShouldHide = false;
		if (auto Comp = MathElements[i]->Cast<MathElementV2::FTAMeComposite>())
		{
			ShowElements(Comp->Children);
		}
	}
}

void FTAMeHelpers::ShowElements(const MathElementV2::FMathElements& MathElements)
{
	ShowElements(MathElements, 0, MathElements.size());
}

void FTAMeHelpers::RedrawIfVisible(const std::vector<TTypedWeak<MathElementV2::FTAMathElementBase>>& MathElements)
{
	for (auto Me : MathElements)
	{
		if (Me->Visual && Me->Visual->IsShown())
		{
			Me->Visual->Show();
		}
	}
}

void FTAMeHelpers::RedrawIfVisible(const MathElementV2::FMathElements& MathElements)
{
	for (auto Me : MathElements)
	{
		if (Me->Visual && Me->Visual->IsShown())
		{
			Me->Visual->Show();
		}
	}
}

void FTAMeHelpers::HighlightContainers(const MathElementV2::FMathElements& MathElements)
{
	for (auto& Me : MathElements)
	{
		if (auto Comp = Me->Cast<MathElementV2::FTAMeComposite>())
		{
			HighlightContainers(Comp->Children);
		}
		if (auto Cont = Me->Cast<MathElementV2::FTAMeContainer>())
		{
			Cont->bHighlight = true;
		}
	}
}

MathElementV2::FMathElements FTAMeHelpers::ParseFromString(const std::wstring& InString, class FTAMathElementGenerator* Generator)
{
	if (!Generator)
	{
		//If cant create character
		return {};
	}
	enum class EState
	{
		Default,
		ReadingMathElement,
		ReadingInfo,
		ExpectChildren
	} State = EState::Default;

	MathElementV2::FMathElements Result;
	MathElementV2::FTAMeComposite* Parent = nullptr;
	MathElementV2::FMathElementPtr LastElement;
	std::wstring MeName;
	std::wstring Info;
	constexpr auto TermChar = L'\\';
	constexpr auto StartChildChar = L'{';
	constexpr auto EndChildChar = L'}';

	for (int i = 0; i < InString.size(); i++)
	{
		if (State == EState::ReadingMathElement)
		{
			//Reading Me
			if (InString[i] == TermChar)
			{
				//End reading Me?
				//start reading info
				Info.clear();
				State = EState::ReadingInfo;
				continue;
			}
			MeName += std::wstring(1, InString[i]);
			continue;
		}
		if (State == EState::ReadingInfo)
		{
			//Reading info
			if (InString[i] == TermChar)
			{
				//Finished info?
				//Try to create Me
				MathElementV2::FMathElementPtr Me = Generator->CreateMathElement(MeName, Info);
				if (!Me)
				{
					//failed to create me
					return {};
				}
				//Successfully created
				Me->SetParent(Parent);
				ResultOrParentArray(Result, Parent).push_back(Me);
				State = EState::ExpectChildren;
				continue;
			}
			Info += std::wstring(1, InString[i]);
			continue;
		}
		if (State == EState::ExpectChildren)
		{
			//Children started with combination "\{"
			if (i + 1 < InString.size() && InString[i] == '\\' && InString[i + 1] == StartChildChar)
			{
				//If start children
				//Set new parent
				Parent = ResultOrParentArray(Result, Parent).back()->Cast<MathElementV2::FTAMeComposite>();
				if (!Parent)
				{
					//If new parent not composite -> fail
					return {};
				}
				//Return to default state
				State = EState::Default;
				//increment to skip two chars
				++i;
				continue;
			}
			//back to default state
			State = EState::Default;
		}
		if (State == EState::Default)
		{
			if (InString[i] == TermChar)
			{
				if (i + 1 < InString.size() && InString[i + 1] == EndChildChar)
				{
					//If end children
					if (!Parent)
					{
						//If no parent -> fail
						return {};
					}
					//set parent one level higher
					Parent = Parent->GetParent().Get();
					//Skip "\" and continue
					++i;
					continue;
				}
				//Start reading Me
				State = EState::ReadingMathElement;
				MeName.clear();
				continue;
			}
			//Read character
			auto Me = Generator->CreateCharacter(InString[i]);
			Me->SetParent(Parent);
			ResultOrParentArray(Result, Parent).push_back(Me);
		}
	}
	if ((State != EState::Default && State != EState::ExpectChildren) || Parent != nullptr)
	{
		return {};
	}
	return Result;
}

std::wstring FTAMeHelpers::ParseToString(const std::vector<TTypedWeak<MathElementV2::FTAMathElementBase>>& MathElements)
{
	std::wstring OutStr;
	for (auto Me : MathElements)
	{
		OutStr += Me->GetParseInfo();
		if (auto Comp = Me->Cast<MathElementV2::FTAMeComposite>())
		{
			OutStr += L"\\{";
			ParseToString(Comp->GetChildren(), OutStr);
			OutStr += L"\\}";
		}
	}
	return OutStr;
}

std::wstring FTAMeHelpers::ParseToString(const MathElementV2::FMathElements& MathElements, int From, int To)
{
	std::wstring OutStr;
	for (int i = From; i < To; i++)
	{
		OutStr += MathElements[i]->GetParseInfo();
		if (auto Comp = MathElements[i]->Cast<MathElementV2::FTAMeComposite>())
		{
			OutStr += L"\\{";
			ParseToString(Comp->GetChildren(), OutStr);
			OutStr += L"\\}";
		}
	}
	return OutStr;
}

void FTAMeHelpers::ParseToString(const MathElementV2::FMathElements& MathElements, std::wstring& OutStr)
{
	for (auto Me : MathElements)
	{
		OutStr += Me->GetParseInfo();
		if (auto Comp = Me->Cast<MathElementV2::FTAMeComposite>())
		{
			OutStr += L"\\{";
			ParseToString(Comp->GetChildren(), OutStr);
			OutStr += L"\\}";
		}
	}
}

int FTAMeHelpers::ScrollY(MathElementV2::FTAMeDocument* InDocument, int Count)
{
	int AbsCount = (std::abs)(Count);
	int LineStart = InDocument->VisibleFrom;
	if (Count < 0)
	{
		//if scroll up
		for (int i = 0; i < AbsCount; i++)
		{
			FindPrevLineStart(InDocument, LineStart, LineStart);
			if (LineStart == -1)
			{
				LineStart = 0;
				break;
			}
		}
	}
	else
	{
		//If scroll down
		for (int i = 0; i < AbsCount; i++)
		{
			//Find line start
			if (MathElementV2::FTAMeNewLine* NewLine = FindLineStart(InDocument, LineStart, LineStart))
			{
				//Cache line start
				int LineStartTemp = LineStart;
				LineStartTemp += NewLine->ElementsCount;
				assert(LineStartTemp <= InDocument->Children.size()); //Something wrong with NewLine->ElementsCount
				if (LineStartTemp == InDocument->Children.size())
				{
					//If this is last line
					return LineStart;
				}
				LineStart = LineStartTemp;
			}
			else
			{
				return InDocument->VisibleFrom;
			}
		}
	}
	return LineStart;
}

void FTAMeHelpers::GetIndexAtPosition(MathElementV2::FTAMeDocument* InDocument, const TACommonTypes::FTAVector2d& Position, FTAMePath& Path)
{
	Path.TreePath.clear();
	for (int i = InDocument->VisibleFrom; i < InDocument->VisibleTo; i++)
	{
		if (IsWithinMe(InDocument->Children[i], Position))
		{
			//Inside some me
			//allocate index in path and cache its position in array
			Path.TreePath.push_back(i);
			int Index = Path.TreePath.size() - 1;
			if (auto Composite = InDocument->Children[i]->Cast<MathElementV2::FTAMeComposite>())
			{
				//If composite
				if (GetIndexAtPosition(Composite->Children, Position, Path))
				{
					//Apply recursion
					return;
				}
			}
			//If failed to find elements recursively
			//Use logic for symbols -> find desired half of an element
			Path.TreePath[Index] = IsLeftHalf(InDocument->Children[i], Position) ? i : i + 1;
			return;
		}
	}
	//If no element was under cursor, find the closest one
	int ClosestIndex = FindClosestIndex(InDocument->Children, Position, InDocument->VisibleFrom, InDocument->VisibleTo);
	if (CommonHelpers::IsValidId(InDocument->Children, ClosestIndex))
	{
		ClosestIndex = IsLeftHalf(InDocument->Children[ClosestIndex], Position) ? ClosestIndex : ClosestIndex + 1;
	}
	Path.TreePath.push_back(ClosestIndex);
}

bool FTAMeHelpers::GetIndexAtPosition(const MathElementV2::FMathElements& MathElements, const TACommonTypes::FTAVector2d& Position, FTAMePath& Path)
{
	for (int i = 0; i < MathElements.size(); i++)
	{
		if (IsWithinMe(MathElements[i], Position))
		{
			//Inside some me
			//allocate index in path and cache its position in array
			Path.TreePath.push_back(i);
			int Index = Path.TreePath.size() - 1;
			if (auto Composite = MathElements[i]->Cast<MathElementV2::FTAMeComposite>())
			{
				//If composite
				if (GetIndexAtPosition(Composite->Children, Position, Path))
				{
					//Apply recursion
					return true;
				}
				//If there are no recursive children under cursor
				if (auto Container = MathElements[i]->Cast<MathElementV2::FTAMeContainer>())
				{
					//But this is container -> find the closest index in it
					int ClosestIndex = FindClosestIndex(Container->Children, Position);
					if (CommonHelpers::IsValidId(Container->Children, ClosestIndex))
					{
						ClosestIndex = IsLeftHalf(Container->Children[ClosestIndex], Position) ? ClosestIndex : ClosestIndex + 1;
					}
					Path.TreePath.push_back(ClosestIndex);
					return true;
				}
			}
			//If failed to find elements recursively
			//Use logic for symbols -> find desired half of an element
			Path.TreePath[Index] = IsLeftHalf(MathElements[i], Position) ? i : i + 1;
			return true;
		}
	}
	return false;
}

int FTAMeHelpers::FindClosestIndex(const MathElementV2::FMathElements& MathElements, const TACommonTypes::FTAVector2d& Position)
{
	return FindClosestIndex(MathElements, Position, 0, MathElements.size());
}

int FTAMeHelpers::FindClosestIndex(const MathElementV2::FMathElements& MathElements, const TACommonTypes::FTAVector2d& Position, int From, int To)
{
	int ClosestIndex = From;
	float ClosestDistance = (std::numeric_limits<float>::max)();
	for (int i = From; i < To; i++)
	{
		float LenSq = ClosestDistSq(MathElements[i], Position);
		if (ClosestDistance > LenSq)
		{
			ClosestIndex = i;
			ClosestDistance = LenSq;
		}
	}
	return ClosestIndex;
}

bool FTAMeHelpers::IsWithinMe(const MathElementV2::FMathElementPtr& Me, const TACommonTypes::FTAVector2d& Position)
{
	auto PosFrom = Me->GetAbsolutePosition();
	auto PosTo = PosFrom + Me->AbsoluteSize;
	return Position.x <= PosTo.x && Position.y <= PosTo.y && Position.x > PosFrom.x && Position.y > PosFrom.y;
}

bool FTAMeHelpers::IsLeftHalf(const MathElementV2::FMathElementPtr& Me, const TACommonTypes::FTAVector2d& Position)
{
	float XNormalized = (Position.x - Me->GetAbsolutePosition().x);
	return XNormalized < Me->AbsoluteSize.x / 2.f;
}

float FTAMeHelpers::ClosestDistSq(const MathElementV2::FMathElementPtr& Me, const TACommonTypes::FTAVector2d& InPos)
{
	TACommonTypes::FTAVector2d AbsolutePosition = Me->GetAbsolutePosition();
	float X1 = AbsolutePosition.x;
	float X2 = X1 + Me->AbsoluteSize.x;
	float Y1 = AbsolutePosition.y;
	float Y2 = Y1 + Me->AbsoluteSize.y;

	TACommonTypes::FTAVector2d PointOnMe;

	bool OnTheLeft = InPos.x < X1;
	bool OnTheRight = InPos.x > X2;
	bool Above = InPos.y < Y1;
	bool Below = InPos.y > Y2;

	if (!OnTheLeft && !OnTheRight)
	{
		//If inside horizontal segment
		return (std::pow)(Above ? Y1 - InPos.y : Y2 - InPos.y, 2);
	}
	if (!Above && !Below)
	{
		//If inside Vertical segment
		return (std::pow)(OnTheLeft ? X1 - InPos.x : X2 - InPos.x, 2);
	}
	PointOnMe.x = OnTheLeft ? X1 : X2;
	PointOnMe.y = Above ? Y1 : Y2;
	return (InPos - PointOnMe).SquaredLength();
}

std::vector<TTypedWeak<MathElementV2::FTAMathElementBase>> FTAMeHelpers::GetSelectedElements(MathElementV2::FTAMeDocument* InDocument, FTAMePath InPath1, FTAMePath InPath2, FTAMePath& OutCursorPos, bool& bPath1From)
{
	std::vector<TTypedWeak<MathElementV2::FTAMathElementBase>> Result;
	if (InPath1.TreePath == InPath2.TreePath)
	{
		OutCursorPos = InPath1;
		return Result;
	}
	//Get path depth for elements with same parent
	FTAMePath SmallerPath;
	FTAMePath LargerPath;
	int SameParentId = GetLastSameParentInd(InPath1, InPath2, SmallerPath, LargerPath);
	//Get this common parent element
	TTypedWeak<MathElementV2::FTAMeComposite> ParentElement = GetParentElement(InDocument, LargerPath, SameParentId);
	if (ParentElement->IsOfType(MathElementV2::FTAMeContainer::StaticType()) || ParentElement->IsOfType(MathElementV2::FTAMeDocument::StaticType()))
	{
		//If parent is container or document
		//Find path From/To
		bPath1From = InPath1.TreePath[SameParentId] < InPath2.TreePath[SameParentId];
		FTAMePath& PathFrom = bPath1From ? InPath1 : InPath2;
		FTAMePath& PathTo = !bPath1From ? InPath1 : InPath2;
		if (PathTo.TreePath.size() > SameParentId + 1)
		{
			//If Path To points at composite element child e.g. Cont of integral 
			//We want to select this composite(Integral itself) also -> incr index to
			PathTo.TreePath[SameParentId]++;
		}
		//Cache selected elements
		for (int i = PathFrom.TreePath[SameParentId]; i < PathTo.TreePath[SameParentId]; i++)
		{
			Result.push_back(ParentElement->Children[i]->GetSelfWeak());
		}
		TrimPath(InPath2, SameParentId + 1);
	}
	else
	{
		//If parent is not container/document -> simply select only it
		Result.push_back(ParentElement);
		//Trim path to point at parent
		TrimPath(InPath2, SameParentId);
		bPath1From = false;
	}
	OutCursorPos = InPath2;
	return Result;
}

void FTAMeHelpers::TrimPath(FTAMePath& OutPath, int NewSize)
{
	if (OutPath.TreePath.size() < NewSize)
	{
		return;
	}
	OutPath.TreePath.resize(NewSize);
}

int FTAMeHelpers::GetLastSameParentInd(const FTAMePath& InPath1, const FTAMePath& InPath2, FTAMePath& SmallerPath, FTAMePath& LargerPath)
{
	SmallerPath = InPath1.TreePath.size() < InPath2.TreePath.size() ? InPath1 : InPath2;
	LargerPath = InPath1.TreePath.size() >= InPath2.TreePath.size() ? InPath1 : InPath2;
	int i = 0;
	for (; i < SmallerPath.TreePath.size(); i++)
	{
		if (SmallerPath.TreePath[i] != LargerPath.TreePath[i])
		{
			return i;
		}
	}
	if (InPath1.TreePath.size() == InPath2.TreePath.size())
	{
		return InPath1.TreePath.size() - 1;
	}
	return i;
}

int FTAMeHelpers::FindElementByX(const MathElementV2::FMathElements& MathElements, int From, float InX)
{
	for (int i = From; i < MathElements.size(); i++)
	{
		if (MathElements[i]->IsOfType(MathElementV2::FTAMeNewLine::StaticType()))
		{
			return i;
		}
		if (MathElements[i]->LocalPosition.x <= InX && MathElements[i]->LocalPosition.x + MathElements[i]->AbsoluteSize.x >= InX)
		{
			return i;
		}
	}
	return MathElements.size();
}

void FTAMeHelpers::FindMe(const std::vector<TTypedWeak<MathElementV2::FTAMathElementBase>>& Template, const MathElementV2::FMathElements& MathElements, int From, int To, std::vector<MathElementV2::FMathElements>& Result)
{
	To = (std::min)(To, int(MathElements.size() - Template.size() + 1));
	for (int i = From; i < To;)
	{
		if (CompareMe(Template, MathElements,i))
		{
			auto& Back = Result.emplace_back();
			Back.insert(Back.begin(),MathElements.begin() + i,MathElements.begin() + i + Template.size());
			i += Template.size();
		}
		else if (auto AsComposite = MathElements[i]->Cast<MathElementV2::FTAMeComposite>())
		{
			auto& Children = AsComposite->GetChildren();
			FindMe(Template, Children,0,Children.size(),Result);
		}
		i++;
	}
}

int FTAMeHelpers::GetLinesCount(const MathElementV2::FMathElements& AddedMe)
{
	return GetLinesCount(AddedMe, 0, AddedMe.size());
}

int FTAMeHelpers::GetLinesCount(const MathElementV2::FMathElements& MathElements, int From, int To)
{
	int Res = 0;
	for (int i = From; i < To; i++)
	{
		Res += MathElements[i]->IsOfType(MathElementV2::FTAMeNewLine::StaticType());
	}
	return Res;
}

void FTAMeHelpers::MakeXStep(MathElementV2::FTAMeDocument* InDocument, FTAMePath& InPath, int Count)
{
	TTypedWeak<MathElementV2::FTAMeComposite> ParentElement = GetParentElement(InDocument, InPath);
	auto Composite = ParentElement->Cast<MathElementV2::FTAMeComposite>();
	int NewIndexInContainer = InPath.TreePath.back() + Count;
	bool bGoOutside = NewIndexInContainer > Composite->Children.size() || NewIndexInContainer < 0;

	if (auto Container = ParentElement->Cast<MathElementV2::FTAMeContainer>())
	{
		//If container
		if (bGoOutside)
		{
			StepOutsideContainer(Container, InPath, Count, true);
			return;
		}
	}
	if (bGoOutside)
	{
		return;
	}
	//Try step inside next or previous element
	//Get NeighbourMe
	std::shared_ptr<MathElementV2::FTAMathElementBase> NeighbourMe = Count > 0
		                                                                 ? Composite->Children[InPath.TreePath.back()]
		                                                                 : Composite->Children[InPath.TreePath.back() - 1];
	if (auto CompNeighbourMe = NeighbourMe->Cast<MathElementV2::FTAMeComposite>())
	{
		//If Neighbour is composite -> Step inside
		InPath.TreePath.back() += Count > 0 ? 0 : -1;
		//Set container index
		InPath.TreePath.push_back(Count > 0 ? 0 : CompNeighbourMe->Children.size() - 1);
		//Set index in container
		if (auto Container = CompNeighbourMe->Children[InPath.TreePath.back()]->Cast<MathElementV2::FTAMeContainer>())
		{
			InPath.TreePath.push_back(Count > 0 ? 0 : Container->Children.size());
		}
		return;
	}
	//If Neighbour is non-composite, simply step over
	InPath.TreePath.back() = NewIndexInContainer;
}

void FTAMeHelpers::MakeYStep(MathElementV2::FTAMeDocument* InDocument, FTAMePath& InPath, int Count)
{
	TTypedWeak<MathElementV2::FTAMeComposite> ParentElement = GetParentElement(InDocument, InPath);
	auto Composite = ParentElement->Cast<MathElementV2::FTAMeComposite>();

	if (auto Container = ParentElement->Cast<MathElementV2::FTAMeContainer>())
	{
		//If container
		StepOutsideContainer(Container, InPath, Count, false);
		return;
	}
	//If Document
	//Try step Up or Down
	//Cache X pos before cursor
	float RightOffset = 5.f;
	float XPos = RightOffset;
	if (CommonHelpers::IsValidId(InDocument->Children, InPath.TreePath.back() - 1))
	{
		//has element before caret
		auto& MeBeforeCaret = InDocument->Children[InPath.TreePath.back() - 1];
		//XPos = Element's right side + little offset
		XPos = MeBeforeCaret->LocalPosition.x + MeBeforeCaret->AbsoluteSize.x + RightOffset;
	}
	if (Count < 0)
	{
		//If step Up
		int LineStart = FindPrevLineStartInd(InDocument, InPath.TreePath.back() - 1);
		InPath.TreePath.back() = FindElementByX(InDocument->Children, LineStart, XPos);
	}
	else
	{
		//If step Down
		int LineStart = FindNextLineStartInd(InDocument, InPath.TreePath.back());
		InPath.TreePath.back() = FindElementByX(InDocument->Children, LineStart, XPos);
	}
}

int FTAMeHelpers::DefaultStep(int Count, int Index, int ElementsCount)
{
	int NewIndex = Index + Count;
	if (NewIndex >= ElementsCount || NewIndex < 0)
	{
		return -1;
	}
	return NewIndex;
}

const std::map<std::wstring, std::wstring>& FTAMeHelpers::GetMathElementsList()
{
	std::wstring Sp = L"\\";
	std::wstring Op = Sp + L"{";
	std::wstring Cl = Sp + L"}";
	static std::map<std::wstring, std::wstring> List =
	{
		{L"Integral", Sp + MathElementV2::FTAMeFromTo::GetName() + Sp + FTAMeDefinition::IntegralInfo + Sp + GenContainers(2)},
		{L"Double Integral", Sp + MathElementV2::FTAMeFromTo::GetName() + Sp + FTAMeDefinition::DoubleIntegralInfo + Sp + GenContainers(2)},
		{L"Triple Integral", Sp + MathElementV2::FTAMeFromTo::GetName() + Sp + FTAMeDefinition::TripleIntegralInfo + Sp + GenContainers(2)},
		{L"Quadruple Integral", Sp + MathElementV2::FTAMeFromTo::GetName() + Sp + FTAMeDefinition::QuadrupleIntegralInfo + Sp + GenContainers(2)},
		{L"Contour Integral", Sp + MathElementV2::FTAMeFromTo::GetName() + Sp + FTAMeDefinition::ContourIntegralInfo + Sp + GenContainers(2)},
		{L"Summation", Sp + MathElementV2::FTAMeFromTo::GetName() + Sp + FTAMeDefinition::SummationInfo + Sp + GenContainers(2)},
		{L"Product", Sp + MathElementV2::FTAMeFromTo::GetName() + Sp + FTAMeDefinition::ProductInfo + Sp + GenContainers(2)},
		{L"Union over", Sp + MathElementV2::FTAMeFromTo::GetName() + Sp + FTAMeDefinition::UnionOverInfo + Sp + GenContainers(2)},

		{L"Doublescript", Sp + MathElementV2::FTAMeIndex::GetName() + Sp + FTAMeDefinition::DoublescriptInfo + Sp + GenContainers(2)},
		{L"Subscript", Sp + MathElementV2::FTAMeIndex::GetName() + Sp + FTAMeDefinition::SubscriptInfo + Sp + GenContainers(1)},
		{L"Superscript", Sp + MathElementV2::FTAMeIndex::GetName() + Sp + FTAMeDefinition::SuperscriptInfo + Sp + GenContainers(1)},
		/*
		{L"Mat3x3", Sp + MathElementV2::FTAMeMatrix::GetName() + Sp + FTAMeDefinition::GetMatInfo(3, 3) + Sp + GenContainers(9)},
		{L"Vec 3", Sp + MathElementV2::FTAMeMatrix::GetName() + Sp + FTAMeDefinition::GetMatInfo(3, 1) + Sp + GenContainers(3)},

		{L"Hat", Sp + MathElementV2::FTAMeHat::GetName() + Sp + FTAMeDefinition::HatInfo + Sp + GenContainers(1)},
		{L"Hat Bar", Sp + MathElementV2::FTAMeHat::GetName() + Sp + FTAMeDefinition::HatBarInfo + Sp + GenContainers(1)},
		{L"Hat Vec", Sp + MathElementV2::FTAMeHat::GetName() + Sp + FTAMeDefinition::HatVecInfo + Sp + GenContainers(1)},

		{L"Single Parameter", Sp + MathElementV2::FTAMeParameter::GetName() + Sp + FTAMeDefinition::SingleParamInfo + Sp + GenContainers(1)},
		{L"Multi Parameter", Sp + MathElementV2::FTAMeParameter::GetName() + Sp + FTAMeDefinition::MultiParamInfo + Sp + GenContainers(1)},

		{L"Fraction", Sp + MathElementV2::FTAMeFraction::GetName() + Sp + Sp + GenContainers(2)},

		{L"Cursor placement", Sp + MathElementV2::FTAMeSpecialSymbol::GetName() + Sp + FTAMeDefinition::CursorPlacementInfo + Sp},
		{L"Insert selected", Sp + MathElementV2::FTAMeSpecialSymbol::GetName() + Sp + FTAMeDefinition::EmbraceSymbolInfo + Sp},
		{L"Expression start", Sp + MathElementV2::FTAMeSpecialSymbol::GetName() + Sp + FTAMeDefinition::ExprStartInfo + Sp},
		{L"Expression end", Sp + MathElementV2::FTAMeSpecialSymbol::GetName() + Sp + FTAMeDefinition::ExprEndInfo + Sp},

		{L"Cases", Sp + MathElementV2::FTAMeCases::GetName() + Sp + FTAMeDefinition::GetCasesInfo(2, 2, FTAMeDefinition::CasesCurly) + Sp + GenContainers(4)},
		{L"Square cases", Sp + MathElementV2::FTAMeCases::GetName() + Sp + FTAMeDefinition::GetCasesInfo(2, 2, FTAMeDefinition::CasesSquare) + Sp + GenContainers(4)},

		{L"Function container", Sp + MathElementV2::FTAMeFunction::GetName() + Sp + Sp + GenContainers(2)},

		{L"Text block", Sp + MathElementV2::FTAMeTextBlock::GetName() + Sp + Sp + GenContainers(1)},

		{L"Non commute", Sp + MathElementV2::FTAMeCustomToken::GetName() + Sp + FTAMeDefinition::NonCommuteInfo + Sp + GenContainers(1)},
		{L"Token Word", Sp + MathElementV2::FTAMeCustomToken::GetName() + Sp + FTAMeDefinition::TokenWord + Sp + GenContainers(1)},
		{L"Token Expression", Sp + MathElementV2::FTAMeCustomToken::GetName() + Sp + FTAMeDefinition::TokenExpression + Sp + GenContainers(1)},*/

		{L"Alpha", L"α"}, {L"CAlpha", L"Α"},
		{L"Beta", L"β"}, {L"CBeta", L"Β"},
		{L"Gamma", L"γ"}, {L"CGamma", L"Γ"},
		{L"Delta", L"δ"}, {L"CDelta", L"Δ"},
		{L"Epsilon", L"ε"}, {L"CEpsilon", L"Ε"},
		{L"Zeta", L"ζ"}, {L"CZeta", L"Ζ"},
		{L"Eta", L"η"}, {L"CEta", L"Η"},
		{L"Theta", L"θ"}, {L"CTheta", L"Θ"},
		{L"Iota", L"ι"}, {L"CIota", L"Ι"},
		{L"Kappa", L"κ"}, {L"CKappa", L"Κ"},
		{L"Lambda", L"λ"}, {L"CLambda", L"Λ"},
		{L"Mu", L"μ"}, {L"CMu", L"Μ"},
		{L"Nu", L"ν"}, {L"CNu", L"Ν"},
		{L"Xi", L"ξ"}, {L"CXi", L"Ξ"},
		{L"Omicron", L"ο"}, {L"COmicron", L"Ο"},
		{L"Pi", L"π"}, {L"CPi", L"Π"},
		{L"Rho", L"ρ"}, {L"CRho", L"Ρ"},
		{L"Sigma", L"σ"}, {L"CSigma", L"Σ"},
		{L"Tau", L"τ"}, {L"CTau", L"Τ"},
		{L"Upsilon", L"υ"}, {L"CUpsilon", L"Υ"},
		{L"Phi", L"φ"}, {L"CPhi", L"Φ"},
		{L"Chi", L"χ"}, {L"CChi", L"Χ"},
		{L"Psi", L"ψ"}, {L"CPsi", L"Ψ"},
		{L"Omega", L"ω"}, {L"COmega", L"Ω"},
		{L"Partial", L"∂"},
		{L"Differential", L"đ"},
		{L"Right Arrow", L"→"},
		{L"Left Arrow", L"←"},
		{L"Not Equal", L"≠"},
		{L"Less Or Equal", L"≤"},
		{L"Greater Or Equal", L"≥"},
		{L"Cross", L"✕"},
		{L"Dot", L"·"},
		{L"Infinity", L"∞"},
		{L"Plus Minus", L"±"},
		{L"Division Sign", L"÷"},
		{L"Square Root", L"√"},
		{L"Wave Equal", L"≈"},
		{L"Triple Bar", L"≡"},
		{L"Isomorphism", L"≅"},
		{L"Empty Set", L"∅"},
		{L"In", L"∈"},
		{L"Not Belong", L"∉"},
		{L"Set Inclusion", L"⊂"},
		{L"Subset", L"⊆"},
		{L"Proper Subset", L"⊊"},
		{L"Union", L"∪"},
		{L"Intersection", L"∩"},
		{L"Symmetric difference", L"⊖"},
		{L"Disjoint union", L"⊔"},
		{L"Not Sign", L"¬"},
		{L"Descending wedge", L"∨"},
		{L"Wedge", L"∧"},
		{L"Exclusive or", L"⊻"},
		{L"For all", L"∀"},
		{L"Exists", L"∃"},
		{L"Implies", L"⇒"},
		{L"Logical equivalence", L"⇔"},
		{L"Tee", L"⊤"},
		{L"Up tack", L"⊥"},
		{L"Natural numbers", L"ℕ"},
		{L"Integers", L"ℤ"},
		{L"Rational numbers", L"ℚ"},
		{L"Real numbers", L"ℝ"},
		{L"Complex numbers", L"ℂ"},
		{L"Function", L"↦"},
		{L"Composition", L"∘"},
		{L"Nabla", L"∇"},
		{L"Direct sum", L"⊕"},
		{L"Tensor product", L"⊗"},
		{L"Dots", L"⋯"},
		{L"VDots", L"⋮"},
		{L"IDots", L"⋰"},
		{L"DDots", L"⋱"},
		{L"<", L"⟨"},
		{L">", L"⟩"},

	};
	return List;
}

std::wstring FTAMeHelpers::GenContainers(int Count)
{
	std::wstring Result;
	Result += L"\\{";
	for (int i = 0; i < Count; i++)
	{
		Result += L"\\" + MathElementV2::FTAMeContainer::GetName() + L"\\\\";
	}
	Result += L"\\}";
	return Result;
}

void FTAMeHelpers::StepOutsideContainer(MathElementV2::FTAMeContainer* InContainer, FTAMePath& InPath, int Count, bool bXDir)
{
	TTypedWeak<MathElementV2::FTAMeComposite> ContainersParent = InContainer->GetParent();
	//Go one level higher
	InPath.TreePath.pop_back();
	//Try to get step component e.g. to handle complex movement like that of matrix
	FTAMeStepComponent* StepComponent = ContainersParent->GetComponent<FTAMeStepComponent>();
	int NextContainerIndex;
	if (StepComponent)
	{
		//If has step component
		//Get next container index using StepComponent rules
		NextContainerIndex = bXDir ? StepComponent->StepX(Count, InPath.TreePath.back()) : StepComponent->StepY(Count, InPath.TreePath.back());
	}
	else
	{
		//Use default logic to get next container
		NextContainerIndex = DefaultStep(Count, InPath.TreePath.back(), ContainersParent->Children.size());
	}
	if (NextContainerIndex != -1)
	{
		//Set new container index
		InPath.TreePath.back() = NextContainerIndex;
		//Set index in container
		auto Container = ContainersParent->Children[NextContainerIndex]->Cast<MathElementV2::FTAMeContainer>();
		assert(Container);
		InPath.TreePath.push_back(Count > 0 ? 0 : Container->Children.size());
		return;
	}
	//If step outside (-1)
	//Go one level higher
	InPath.TreePath.pop_back();
	//Step right
	InPath.TreePath.back() += Count > 0 ? 1 : 0;
}

MathElementV2::FMathElements& FTAMeHelpers::ResultOrParentArray(MathElementV2::FMathElements& Result, MathElementV2::FTAMeComposite* Parent)
{
	if (Parent)
	{
		return Parent->GetChildren();
	}
	return Result;
}
