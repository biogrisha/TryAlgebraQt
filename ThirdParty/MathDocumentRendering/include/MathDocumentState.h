#pragma once
#include "MathDocumentRenderingStructs.h"
class FFreeTypeWrap;
class FMathDocumentState
{
public:
	void CopyChanged(const FMathDocumentState& From);
	FMathDocumentState& ClearText();
	FMathDocumentState& ClearSelection();
	FMathDocumentState& ClearCosmeticRects();
	void Invalidate();
	void AddGlyph(const FGlyphData& Glyph);
	void appendGlyphs(const std::vector<FGlyphData>& glyphs);
	void SetCaret(const FCaretData& InCaretData);
	void AddSelection(const FRectInst& Rect);
	void AddCosmeticRect(const FRectInst& Rect);
	void Update();
	bool IsTextUpdated();
	bool IsCaretUpdated();
	bool IsRectsUpdated();
	const std::vector<FGlyphData>& GetText();
	const FCaretData& GetCaretData();
	const std::vector<FRectInst>& GetSelectionRects();
	const std::vector<FRectInst>& GetCosmeticRects();
private:
	std::vector<FGlyphData> Text;
	FCaretData CaretData;
	std::vector<FRectInst> Selection;
	std::vector<FRectInst> Cosmetic;

	bool bTextUpdated = false;
	bool bCaretUpdated = false;
	bool bSelectionUpdated = false;
	bool bCosmeticRectsUpdated = false;
};

struct VisualToolkit
{
	FMathDocumentState* mdocState = nullptr;
	FFreeTypeWrap* ft = nullptr;
};