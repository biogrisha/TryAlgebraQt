#pragma once
#include "MathDocumentRenderingStructs.h"
#include <mutex>

class FFreeTypeWrap;

class Layer
{
public:
	void clear();
	void addGlyph(const FGlyphData& glyph);
	void addSprite(const FSpriteInstByName& sprite);
	void addRectangle(const FRectInst& rect);
	void addLine(LineChain lineChain);
	void markClean();
	bool dirty();
	const std::vector<FGlyphData>& text() const;
	const std::vector<FSpriteInstByName>& sprites() const;
	const std::vector<FRectInst>& rectangles() const;
	const std::vector<LineChain>& lines() const;
private:
	std::vector<FGlyphData> m_text;
	std::vector<FSpriteInstByName> m_sprites;
	std::vector<FRectInst> m_rectangles;
	std::vector<LineChain> m_lines;
	bool m_dirty = true;
};

class FMathDocumentState
{
public:
	FMathDocumentState(int layersCount);
	Layer& at(int i);
	std::mutex& mtx();
private:
	std::vector<Layer> m_layers;
	mutable std::mutex m_mtx;
};

struct VisualToolkit
{
	FMathDocumentState* mdocState = nullptr;
	FFreeTypeWrap* ft = nullptr;
};