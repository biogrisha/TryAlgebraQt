#pragma once
#include <MathDocumentState.h>
namespace TryAlgebraCore
{
	struct TreeDisplayItem
	{
		glm::vec2 pos;
		std::wstring label;

		TreeDisplayItem* parent = nullptr;
		std::vector<std::unique_ptr<TreeDisplayItem>> children;
		void draw(const VisualToolkit& vt);
		glm::vec2 calculateSize(const VisualToolkit& vt);
	};


	class TreeDisplay
	{
	public:
		TreeDisplay(const VisualToolkit& vt);

		template<class TermT>
		void setTree(const std::vector<std::unique_ptr<TermT>>& terms)
		{
			m_items.clear();
			toTreeDisplayItems(terms, m_items, nullptr);
			calculate();
		}

		void move(const glm::vec2& pos);
		void draw();
	private:
		template<class TermT>
		void toTreeDisplayItems(const std::vector<std::unique_ptr<TermT>>& terms
			, std::vector<std::unique_ptr<TreeDisplayItem>>& result, TreeDisplayItem* parent = nullptr)
		{
			for (const auto& term : terms)
			{
				auto& newItem = result.emplace_back(std::make_unique<TreeDisplayItem>());
				newItem->label = term->label;
				newItem->parent = parent;
				toTreeDisplayItems(term->children, newItem->children, newItem.get());
			}
		}

		void calculate();
		std::vector<std::unique_ptr<TreeDisplayItem>> m_items;
		VisualToolkit m_vt;
	};
}