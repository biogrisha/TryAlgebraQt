#pragma once
#include <MathDocumentState.h>

namespace TryAlgebraCore
{
	struct TreeDisplayItem
	{
		glm::vec2 pos;
		std::wstring string;

		TreeDisplayItem* parent = nullptr;
		std::vector<std::unique_ptr<TreeDisplayItem>> children;
		void draw(const VisualToolkit& vt);
		glm::vec2 calculateSize(const VisualToolkit& vt);
	};

	class TreeDisplay
	{
		void draw();
	private:
		std::unique_ptr<TreeDisplayItem> m_root;
		VisualToolkit m_vt;
		glm::vec2 pos;
	};
}