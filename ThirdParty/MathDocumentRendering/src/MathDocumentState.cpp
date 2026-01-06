#include "MathDocumentState.h"

void FMathDocumentState::CopyChanged(const FMathDocumentState& Other)
{
	bTextUpdated = Other.bTextUpdated;
	bCaretUpdated = Other.bCaretUpdated;
	bRectsUpdated = Other.bRectsUpdated;
	if (Other.bTextUpdated)
	{
		Text = Other.Text;
	}
	if (Other.bCaretUpdated)
	{
		CaretData = Other.CaretData;
	}
	if (Other.bRectsUpdated)
	{
		Rects = Other.Rects;
	}
}

void FMathDocumentState::Clear(bool bText, bool bRects)
{
	if (bText)
	{
		Text.clear();
		bTextUpdated = true;
	}
	if (bRects)
	{
		Rects.clear();
		bRectsUpdated = true;
	}
}

void FMathDocumentState::AddGlyph(const FGlyphData& Glyph)
{
	bTextUpdated = true;
	Text.push_back(Glyph);
}

void FMathDocumentState::SetCaret(const FCaretData& InCaretData)
{
	bCaretUpdated = true;
	CaretData = InCaretData;
}

void FMathDocumentState::AddRect(const FRectInst& Rect)
{
	bRectsUpdated = true;
	Rects.push_back(Rect);
}

void FMathDocumentState::Update()
{
	bTextUpdated = false;
	bCaretUpdated = false;
	bRectsUpdated = false;
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
	return bRectsUpdated;
}

const std::vector<FGlyphData>& FMathDocumentState::GetText()
{
	return Text;
}

const FCaretData& FMathDocumentState::GetCaretData()
{
	return CaretData;
}

const std::vector<FRectInst>& FMathDocumentState::GetRects()
{
	return Rects;
}
