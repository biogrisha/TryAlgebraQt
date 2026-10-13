#include <Me/include/MeMatrix.h>
#include <Me/include/MeContainer.h>
#include <Helpers/include/MeHelpers.h>
#include <algorithm>
#include <cassert>
#include <vector>

namespace TryAlgebraCore {
	void MeMatrix::setMeta(const std::wstring& meta)
	{
		const size_t separator = meta.find(L',');
		assert(separator != std::wstring::npos);
		m_rows = std::stoi(meta.substr(0, separator));
		m_columns = std::stoi(meta.substr(separator + 1));
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

		std::vector<float> column_widths(m_columns, 0.f);
		std::vector<float> row_heights(m_rows, 0.f);
		const float padding = 5.f * m_scaling_factor;

		// Calculate cell contents using the matrix scaling factor.
		for (auto& ch : m_children)
		{
			if (auto* cont = MyRTTI::Cast<MeContainer>(ch.get()))
			{
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
				column_widths[column] = std::max(column_widths[column], child->getSize().x);
				row_heights[row] = std::max(row_heights[row], child->getSize().y);
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
				child->setPos({ x, y });
				child->setSize({ column_widths[column], row_heights[row] });
				x += column_widths[column];
			}
			y += row_heights[row];
		}

		// Include the outer padding in the final matrix dimensions.
		float width = 0.f;
		for (float column_width : column_widths)
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
			auto contPos = MeHelpers::contPos(this, path).value();

			if (dir == StepDir::left)
			{
				if (contPos == 0)
				{
					path.pop_back();
					return;
				}
			}
		}
	}
}
