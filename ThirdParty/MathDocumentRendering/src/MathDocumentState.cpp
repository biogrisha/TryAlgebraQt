#include "MathDocumentState.h"

void Layer::clear()
{
	m_text.clear();
	m_sprites.clear();
	m_rectangles.clear();
	m_lines.clear();

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

bool Layer::dirty()
{
	return m_dirty;
}

const std::vector<FGlyphData>& Layer::text() const
{
	return m_text;
}

const std::vector<FSpriteInstByName>& Layer::sprites() const
{
	return m_sprites;
}

const std::vector<FRectInst>& Layer::rectangles() const
{
	return m_rectangles;
}

const std::vector<LineChain>& Layer::lines() const
{
	return m_lines;
}

FMathDocumentState::FMathDocumentState(int layersCount)
{
	m_layers.resize(layersCount);
}

Layer& FMathDocumentState::at(int i)
{
	return m_layers[i];
}

std::mutex& FMathDocumentState::mtx()
{
	return m_mtx;
}