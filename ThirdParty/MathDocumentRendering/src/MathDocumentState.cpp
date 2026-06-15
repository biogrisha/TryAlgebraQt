#include "MathDocumentState.h"

void FMathDocumentState::CopyChanged(const FMathDocumentState& Other)
{
	bTextUpdated = Other.bTextUpdated;
	bCaretUpdated = Other.bCaretUpdated;
	bSelectionUpdated = Other.bSelectionUpdated;
	bCosmeticRectsUpdated = Other.bCosmeticRectsUpdated;

	if (Other.bTextUpdated)
	{
		Text = Other.Text;
	}
	if (Other.bCaretUpdated)
	{
		CaretData = Other.CaretData;
	}
	if (Other.bSelectionUpdated)
	{
		Selection = Other.Selection;
	}
	if (Other.bCosmeticRectsUpdated)
	{
		Cosmetic = Other.Cosmetic;
	}

}

FMathDocumentState& FMathDocumentState::ClearText()
{
	if(!Text.empty())
	{
		Text.clear();
		bTextUpdated = true;
	}
	return *this;
}

FMathDocumentState& FMathDocumentState::ClearSelection()
{
	if(!Selection.empty())
	{
		Selection.clear();
		bSelectionUpdated = true;
	}
	return *this;
}

FMathDocumentState& FMathDocumentState::ClearCosmeticRects()
{
	if(!Cosmetic.empty())
	{
		Cosmetic.clear();
		bCosmeticRectsUpdated = true;
	}
	return *this;
}

void FMathDocumentState::Invalidate()
{
	bTextUpdated 
	= bSelectionUpdated 
	= bCaretUpdated 
	= bCosmeticRectsUpdated 
	= true;
}

void FMathDocumentState::AddGlyph(const FGlyphData& Glyph)
{
	bTextUpdated = true;
	Text.push_back(Glyph);
}

void FMathDocumentState::appendGlyphs(const std::vector<FGlyphData>& glyphs)
{
	bTextUpdated = true;
	Text.insert(Text.end(), glyphs.begin(), glyphs.end());
}

void FMathDocumentState::SetCaret(const FCaretData& InCaretData)
{
	bCaretUpdated = true;
	CaretData = InCaretData;
}

void FMathDocumentState::AddSelection(const FRectInst& Rect)
{
	Selection.push_back(Rect);
	bSelectionUpdated = true;
}

void FMathDocumentState::AddCosmeticRect(const FRectInst& Rect)
{
	Cosmetic.push_back(Rect);
	bCosmeticRectsUpdated = true;
}

void FMathDocumentState::Update()
{
	bTextUpdated 
	= bSelectionUpdated 
	= bCaretUpdated 
	= bCosmeticRectsUpdated 
	= false;
}

bool FMathDocumentState::IsTextUpdated()
{
	return bTextUpdated;
}

bool FMathDocumentState::IsCaretUpdated()
{
	return bCaretUpdated;
}

bool FMathDocumentState::IsRectsUpdated()
{
	return bSelectionUpdated || bCosmeticRectsUpdated;
}

const std::vector<FGlyphData>& FMathDocumentState::GetText()
{
	return Text;
}

const FCaretData& FMathDocumentState::GetCaretData()
{
	return CaretData;
}

const std::vector<FRectInst>& FMathDocumentState::GetSelectionRects()
{
	return Selection;
}

const std::vector<FRectInst>& FMathDocumentState::GetCosmeticRects()
{
	return Cosmetic;
}
