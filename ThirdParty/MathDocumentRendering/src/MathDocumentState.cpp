#include "MathDocumentState.h"

void Layer::clear()
{
	*this = Layer();
	m_dirty = true;
}

void Layer::addGlyph(const FGlyphData& glyph)
{
	m_text.push_back(glyph);
	m_dirty = true;
}

void Layer::addSprite(const FSpriteInstByName& sprite)
{
	m_sprites.push_back(sprite);
	m_dirty = true;
}

void Layer::addRectangle(const FRectInst& rect)
{
	m_rectangles.push_back(rect);
	m_dirty = true;
}

void Layer::addLine(const LineChain& lineChain)
{
	m_lines.push_back(lineChain);
	m_dirty = true;
}

void Layer::markClean()
{
	m_dirty = false;
}

FMathDocumentState::FMathDocumentState(int layersCount)
{
	m_layers.resize(layersCount);
}

Layer& FMathDocumentState::at(int i)
{
	return m_layers[i];
}