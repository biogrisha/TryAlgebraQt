#pragma once
#include "MathDocumentRenderingStructs.h"

class FMathDocumentState
{
public:
	void CopyChanged(const FMathDocumentState& From);
	void Clear(bool bText, bool bRects);
	void AddGlyph(const FGlyphData& Glyph);
	void SetCaret(const FCaretData& InCaretData);
	void AddRect(const FRectInst& Rect);
	void Update();
	bool IsTextUpdated();
	bool IsCaretUpdated();
	bool IsRectsUpdated();
	const std::vector<FGlyphData>& GetText();
	const FCaretData& GetCaretData();
	const std::vector<FRectInst>& GetRects();
private:
	std::vector<FGlyphData> Text;
	FCaretData CaretData;
	std::vector<FRectInst> Rects;

	bool bTextUpdated = false;
	bool bCaretUpdated = false;
	bool bRectsUpdated = false;
};