#include <MathEditor/include/TreeDisplay.h>
#include <FreeTypeWrap.h>

namespace
{
	void collectLevels(TryAlgebraCore::TreeDisplayItem* item, int depth, std::vector<std::vector<TryAlgebraCore::TreeDisplayItem*>>& levels)
	{
		if (levels.size() <= depth)
		{
			levels.emplace_back();
		}
		levels[depth].push_back(item);
		for (auto& ch : item->children)
		{
			collectLevels(ch.get(), depth + 1, levels);
		}
	}
}

namespace TryAlgebraCore
{

	void TreeDisplayItem::draw(const VisualToolkit& vt)
	{
		glm::vec2 gPos = pos;
		for (auto ch : string)
		{
			FGlyphData g;
			g.GlyphId.Glyph = ch;
			g.GlyphId.Height = 12;
			g.Pos = gPos;

			auto gSize = vt.ft->GetGlyphSize(g.GlyphId);
			gPos.x += gSize.x;
			vt.mdocState->at(1).addGlyph(g);
		}
		for (auto& ch : children)
		{
			ch->draw(vt);
		}
	}

	glm::vec2 TreeDisplayItem::calculateSize(const VisualToolkit& vt)
	{
		glm::vec2 res = { 0,0 };
		for (auto ch : string)
		{
			FGlyphId g;
			g.Glyph = ch;
			g.Height = 12;
			auto gSize = vt.ft->GetGlyphSize(g);
			res.x += gSize.x;
			res.y = std::max(res.y, gSize.y);
		}
		return res;
	}

	void TreeDisplay::draw()
	{
		m_root->draw(m_vt);
		std::vector<std::vector<TreeDisplayItem*>> levels;
		collectLevels(m_root.get(), 0, levels);

		glm::vec2 pos = { 0,0 };
		TreeDisplayItem* prevParent = levels.back().front()->parent;
		for (auto* item : levels.back())
		{
			if (item->parent != prevParent)
			{
				pos.x += 100;
				prevParent = item->parent;
			}
			item->pos = pos;
			pos.x += item->calculateSize(m_vt).x;
		}

		for (int i = levels.size() - 1; i >= 0; --i)
		{
			for (auto item : levels[i])
			{
				item->pos /= item->children.size();
				item->parent->pos += item->pos;
			}
		}
	}
}
