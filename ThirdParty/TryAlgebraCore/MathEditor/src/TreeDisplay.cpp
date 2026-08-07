#include <MathEditor/include/TreeDisplay.h>
#include <FreeTypeWrap.h>

namespace TryAlgebraCore
{
	namespace
	{
		void collectBottom(TreeDisplayItem* item, std::vector<TreeDisplayItem*>& bottom)
		{
			if (item->children.empty())
			{
				bottom.push_back(item);
				return;
			}
			for (auto& ch : item->children)
			{
				collectBottom(ch.get(), bottom);
			}
		}
		void adjustPositions(std::vector<std::unique_ptr<TreeDisplayItem>>& items, int depth = 0)
		{
			for (auto& item : items)
			{
				if (!item->children.empty())
				{
					adjustPositions(item->children, depth + 1);
					for (auto& ch : item->children)
					{
						item->pos.x += ch->pos.x;
					}
					item->pos /= item->children.size();
				}
				item->pos.y = depth * 100;
			}
		}

		void moveRec(std::vector<std::unique_ptr<TreeDisplayItem>>& items, const glm::vec2& pos)
		{
			for (auto& item : items)
			{
				item->pos += pos;
				moveRec(item->children, pos);
			}
		}

	}


	void TreeDisplayItem::draw(const VisualToolkit& vt)
	{
		glm::vec2 gPos = pos;
		for (auto& ch : label)
		{
			FGlyphData g;
			g.GlyphId.Glyph = ch;
			g.GlyphId.Height = 20;
			g.Pos = gPos;

			auto gSize = vt.ft->GetGlyphSize(g.GlyphId);
			gPos.x += gSize.x;
			vt.mdocState->at(1).addGlyph(g);
		}
		auto size = calculateSize(vt);
		for (auto& ch : children)
		{
			LineChain lineChain;
			lineChain.color = { 1,1,1,1 };
			lineChain.width = 1;
			lineChain.points.push_back(pos + glm::vec2{ size.x / 2, size.y });
			auto chSize = ch->calculateSize(vt);
			lineChain.points.push_back(ch->pos + glm::vec2{ chSize.x / 2, 0 });
			vt.mdocState->at(1).addLine(lineChain);
			ch->draw(vt);
		}
	}

	glm::vec2 TreeDisplayItem::calculateSize(const VisualToolkit& vt)
	{
		glm::vec2 res = { 0,0 };
		for (auto& ch : label)
		{
			FGlyphId g;
			g.Glyph = ch;
			g.Height = 20;
			auto gSize = vt.ft->GetGlyphSize(g);
			res.x += gSize.x;
			res.y = std::max(res.y, gSize.y);
		}
		return res;
	}

	TreeDisplay::TreeDisplay(const VisualToolkit& vt)
		: m_vt(vt)
	{

	}

	void TreeDisplay::move(const glm::vec2& pos)
	{
		moveRec(m_items, pos);
	}

	void TreeDisplay::calculate()
	{
		std::vector<TreeDisplayItem*> bottom;
		for (const auto& item : m_items)
		{
			collectBottom(item.get(), bottom);
		}

		glm::vec2 pos = { 0,0 };
		TreeDisplayItem* prevParent = bottom.front()->parent;
		for (auto* item : bottom)
		{
			item->pos = pos;
			float maxWidth = item->calculateSize(m_vt).x;
			TreeDisplayItem* parent = item->parent;
			while (parent)
			{
				maxWidth = std::max(parent->calculateSize(m_vt).x, maxWidth);
				parent = parent->parent;
			}
			pos.x += maxWidth + 10;
		}

		adjustPositions(m_items);
	}

	void TreeDisplay::draw()
	{
		std::lock_guard<std::mutex> guard(m_vt.mdocState->mtx());
		m_vt.mdocState->at(0).clear();
		m_vt.mdocState->at(1).clear();
		for (const auto& item : m_items)
		{
			item->draw(m_vt);
		}
	}
}
