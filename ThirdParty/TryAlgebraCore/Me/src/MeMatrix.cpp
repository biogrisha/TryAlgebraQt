#include <Me/include/MeMatrix.h>
#include <Me/include/MeGlobals.h>
#include <Me/include/MeContainer.h>
#include <Helpers/include/MeHelpers.h>
#include <algorithm>
#include <cassert>
#include <vector>

namespace TryAlgebraCore {
	void MeMatrix::setMeta(const std::wstring& meta)
	{
		auto matSize = MeNames::matrixSize(meta);
		m_rows = matSize.rows;
		m_columns = matSize.columns;
	}
	void MeMatrix::calculate(VisualToolkit* visual_toolkit)
	{
		// A matrix must have one child container for every cell.
		assert(m_rows > 0);
		assert(m_columns > 0);
		assert(m_children.size() == static_cast<size_t>(m_rows * m_columns));

		// Keep release builds safe when matrix metadata or children are invalid.
		if (m_rows <= 0 || m_columns <= 0
			|| m_children.size() != static_cast<size_t>(m_rows * m_columns))
		{
			setSize({ 0.f, 0.f });
			setBearing(0.f);
			return;
		}

		struct RowSizeData
		{
			float spaceAbove = 0.f;
			float spaceBelow = 0.f;
		};

		std::vector<float> columnWidths(m_columns, 0.f);
		std::vector<RowSizeData> rowSizes(m_rows, {});
		std::vector<float> contSpacesAbove(m_children.size(), 0.f);
		const float padding = 5.f * m_scaling_factor;

		// Calculate cell contents using the matrix scaling factor.
		for (auto& ch : m_children)
		{
			if (auto* cont = MyRTTI::Cast<MeContainer>(ch.get()))
			{
				cont->setCenterContent(true);
				cont->setDrawBorder(true);
			}
			ch->setScalingFactor(m_scaling_factor);
			ch->calculate(visual_toolkit);
		}

		// Each column and row is sized to its largest cell content.
		for (int row = 0; row < m_rows; ++row)
		{
			for (int column = 0; column < m_columns; ++column)
			{
				const auto& child = m_children[row * m_columns + column];
				columnWidths[column] = std::max(columnWidths[column], child->getSize().x);
				if (child->getChildren().empty())
				{
					rowSizes[row].spaceAbove = std::max(rowSizes[row].spaceAbove,
						child->getSize().y / 2);
					contSpacesAbove[row * m_columns + column] = child->getSize().y / 2;
					rowSizes[row].spaceBelow = std::max(rowSizes[row].spaceBelow, child->getSize().y / 2);
				}
				else
				{
					auto& firstChild = child->getChildren().front();
					float spaceAbove = firstChild->getPos().y + firstChild->getBearingY();
					contSpacesAbove[row * m_columns + column] = spaceAbove;
					rowSizes[row].spaceAbove = std::max(rowSizes[row].spaceAbove,
						spaceAbove);
					rowSizes[row].spaceBelow = std::max(rowSizes[row].spaceBelow, child->getSize().y - spaceAbove);
				}

			}
		}

		// Arrange cells in row-major order and expand every container to its cell.
		float y = padding;
		for (int row = 0; row < m_rows; ++row)
		{
			float x = padding;
			for (int column = 0; column < m_columns; ++column)
			{
				auto& child = m_children[row * m_columns + column];
				auto oldPos = child->getPos();
				auto oldSize = child->getSize();
				child->setPos({ x, y });
				child->setSize({ columnWidths[column], rowSizes[row].spaceAbove + rowSizes[row].spaceBelow });
				for (auto& contCh : child->getChildren())
				{
					contCh->setPosX(contCh->getPos().x + (child->getSize().x - oldSize.x) / 2.f);
					contCh->setPosY(contCh->getPos().y + rowSizes[row].spaceAbove - contSpacesAbove[row * m_columns + column]);
				}
				x += columnWidths[column];
			}
			y += rowSizes[row].spaceAbove + rowSizes[row].spaceBelow;
		}

		// Include the outer padding in the final matrix dimensions.
		float width = 0.f;
		for (float column_width : columnWidths)
		{
			width += column_width;
		}

		setSize({ width + padding * 2.f, y + padding });
		setBearing(getSize().y / 2.f);
	}
	void MeMatrix::step(StepDir dir, StepFrom stepFrom, MePath& path)
	{
		if (stepFrom == StepFrom::outside)
		{
			//path points at this
			path.back() = MePos(getChFrom(), getChTo());
			if (dir == StepDir::left)
			{
				auto& lastCont = getChildren().back();
				auto& lastContChildren = lastCont->getChildren();
				path.push_back(ContPos(lastCont->getChFrom()));
				if (lastContChildren.empty())
				{
					path.push_back(LeafPos(lastCont->getChFrom()));
				}
				else
				{
					path.push_back(LeafPos(lastContChildren.back()->getChTo()));
				}
			}
			else if (dir == StepDir::right)
			{
				path.push_back(ContPos(getChildren().front()->getChFrom()));
				path.push_back(LeafPos(getChildren().front()->getChFrom()));
			}
		}
		if (stepFrom == StepFrom::inside)
		{
			path.pop_back();
			auto contPos = MeHelpers::contPos(this, path);

			if (dir == StepDir::left)
			{
				if (contPos % m_columns == 0)
				{
					path.pop_back();
					MeHelpers::mePosToLeaf(path, true);
					return;
				}
				MeHelpers::setContPos(m_children[contPos - 1].get(), path);
				path.push_back(LeafPos());
				MeHelpers::setExtremeLeafPos(m_children[contPos - 1].get(), path, true);
				return;
			}

			if (dir == StepDir::right)
			{
				if (contPos % m_columns == m_columns - 1)
				{
					path.pop_back();
					MeHelpers::mePosToLeaf(path, false);
					return;
				}
				MeHelpers::setContPos(m_children[contPos + 1].get(), path);
				path.push_back(LeafPos());
				MeHelpers::setExtremeLeafPos(m_children[contPos + 1].get(), path, false);
				return;
			}

			if (dir == StepDir::up)
			{
				if (contPos < m_columns)
				{
					path.pop_back();
					MeHelpers::mePosToLeaf(path, true);
					return;
				}
				MeHelpers::setContPos(m_children[contPos - m_columns].get(), path);
				path.push_back(LeafPos());
				MeHelpers::setExtremeLeafPos(m_children[contPos - m_columns].get(), path, false);
				return;
			}

			if (dir == StepDir::down)
			{
				if (contPos >= m_columns * (m_rows - 1))
				{
					path.pop_back();
					MeHelpers::mePosToLeaf(path, false);
					return;
				}
				MeHelpers::setContPos(m_children[contPos + m_columns].get(), path);
				path.push_back(LeafPos());
				MeHelpers::setExtremeLeafPos(m_children[contPos + m_columns].get(), path, false);
				return;
			}

		}
	}
}
