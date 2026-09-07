#include "MathDocumentState.h"

void Layer::clear()
{
	m_text.clear();
	m_sprites.clear();
	m_rectangles.clear();
	m_lines.clear();
	m_customGlyphsInstances.clear();
	m_customGlyphsOutline.clear();
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

void Layer::addLine(LineChain lineChain)
{
	m_lines.push_back(std::move(lineChain));
	m_dirty = true;
}

void Layer::addCustomGlyph(std::vector<FOutlineCurvePoints> outline, const glm::vec2& pos, const glm::vec2& size)
{
	for (auto& curve : outline)
	{
		curve.points[0] *= 64.0f;
		curve.points[1] *= 64.0f;
		curve.points[2] *= 64.0f;
		std::swap(curve.points[0], curve.points[2]);
		curve.a = curve.points[0].y - 2 * curve.points[1].y + curve.points[2].y;
		curve.b = curve.points[0].y - curve.points[1].y;
	}
	auto& inst = m_customGlyphsInstances.emplace_back();
	inst.CurvesCount = outline.size();
	inst.StartIndex = m_customGlyphsOutline.size();
	inst.Size = size;
	inst.Offset = pos;
	m_customGlyphsOutline.insert(m_customGlyphsOutline.end(), outline.begin(), outline.end());
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

const std::vector<FOutlineCurvePoints>& Layer::customGlyphsOutline() const
{
	return m_customGlyphsOutline;
}

const std::vector<FGlyphInstance>& Layer::customGlyphsInstances() const
{
	return m_customGlyphsInstances;
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

void FMathDocumentState::clear()
{
	for (auto& layer : m_layers)
	{
		layer.clear();
	}
}